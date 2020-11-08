// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include "backend.h"

#include <iostream>
#include <map>
#include <set>

namespace arabilis {

Visitor::~Visitor() noexcept {
}

class VariableUsage: public Visitor {
public:
    explicit VariableUsage(std::string filename) noexcept:
            m_filename { std::move(filename) } {
    }

    VariableUsage(const VariableUsage&) noexcept = delete;
    VariableUsage& operator=(const VariableUsage&) noexcept = delete;

    VariableUsage(VariableUsage&&) noexcept = default;
    VariableUsage& operator=(VariableUsage&&) noexcept = default;

    ~VariableUsage() noexcept override = default;

    void operator()(const AddressOfExpression*) override;
    void operator()(const BinOpExpression*) override;
    void operator()(const BreakStatement*) override;
    void operator()(const CallExpression*) override;
    void operator()(const ContinueStatement*) override;
    void operator()(const ExpressionStatement*) override;
    void operator()(const ForStatement*) override;
    void operator()(const Function*) override;
    void operator()(const GlobalVar*) override;
    void operator()(const IfStatement*) override;
    void operator()(const LetStatement*) override;
    void operator()(const NumeralExpression*) override;
    void operator()(const Program*) override;
    void operator()(const ReturnStatement*) override;
    void operator()(const StringExpression*) override;
    void operator()(const UnOpExpression*) override;
    void operator()(const VariableExpression*) override;
    void operator()(const VarStatement*) override;
    void operator()(const WhileStatement*) override;

private:
    std::string m_filename;
    bool m_inside_loop = false;
    std::set<std::string> m_global_symbols = {};
    std::set<std::string> m_local_symbols = {};

    /** Create a nested scope. */
    VariableUsage with_block_scope(bool inside_loop) noexcept;

    /** Check that a global variable is unique. */
    void unique_global(const Position&, const std::string&) noexcept;

    /** Check that a local variable is unique in the current context. */
    void unique_local(const Position&, const std::string&) noexcept;

    /** Check that a name refers to either a local or global variable. */
    void check_variable(const Position&, const std::string&) noexcept;
};

void check_variable_usage(Program& program) noexcept {
    VariableUsage variable_usage { program.m_filename };
    program.visit(variable_usage);
}

void VariableUsage::operator()(const AddressOfExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const BinOpExpression* node) {
    node->m_lhs->visit(*this);
    node->m_rhs->visit(*this);
}

void VariableUsage::operator()(const BreakStatement* node) {
    if (m_inside_loop) {
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << node->m_position
        << ": Error: \"break\" outside of FOR or WHILE loop\n";
    std::exit(1);
}

void VariableUsage::operator()(const CallExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
    for (auto& argument : node->m_arguments) {
        argument->visit(*this);
    }
}

void VariableUsage::operator()(const ContinueStatement* node) {
    if (m_inside_loop) {
        return;
    }
    std::cerr
        << m_filename
        << ':'
        << node->m_position
        << ": Error: \"continue\" outside of FOR or WHILE loop\n";
    std::exit(1);
}

void VariableUsage::operator()(const ExpressionStatement* node) {
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const ForStatement* node) {
    node->m_initial->visit(*this);

    VariableUsage inner = with_block_scope(true);
    inner.unique_local(node->m_position, node->m_variable_name);

    node->m_condition->visit(inner);
    node->m_update->visit(inner);

    for (auto& statement : node->m_statements) {
        statement->visit(inner);
    }
}

void VariableUsage::operator()(const Function* node) {
    unique_global(node->m_position, node->m_name);

    m_local_symbols.clear();

    for (auto& argument : node->m_arguments) {
        unique_local(node->m_position, argument);
    }

    for (auto& statement : node->m_statements) {
        statement->visit(*this);
    }
}

void VariableUsage::operator()(const GlobalVar* node) {
    unique_global(node->m_position, node->m_name);
}

void VariableUsage::operator()(const IfStatement* node) {
    node->m_condition->visit(*this);

    VariableUsage inner_then = with_block_scope(m_inside_loop);
    for (auto& statement : node->m_then_statements) {
        statement->visit(inner_then);
    }

    VariableUsage inner_else = with_block_scope(m_inside_loop);
    for (auto& statement : node->m_else_statements) {
        statement->visit(inner_else);
    }
}

void VariableUsage::operator()(const LetStatement* node) {
    check_variable(node->m_position, node->m_variable_name);
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const NumeralExpression* /* node */) {
}

void VariableUsage::operator()(const Program* node) {
    for (auto& globalvar : node->m_globalvars) {
        globalvar.visit(*this);
    }

    for (auto& function : node->m_functions) {
        function.visit(*this);
    }

    const bool has_main = [node]() {
        for (auto& f : node->m_functions) {
            if (f.m_name == "main") {
                return true;
            }
        }
        return false;
    }();

    if (has_main) {
        return;
    }

    std::cerr << m_filename << ": Error: missing \"main\" function\n";
    std::exit(1);
}

void VariableUsage::operator()(const ReturnStatement* node) {
    node->m_expression->visit(*this);
}

void VariableUsage::operator()(const StringExpression* /* node */) {
}

void VariableUsage::operator()(const UnOpExpression* node) {
    node->m_rhs->visit(*this);
}

void VariableUsage::operator()(const VariableExpression* node) {
    check_variable(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const VarStatement* node) {
    node->m_expression->visit(*this);
    unique_local(node->m_position, node->m_variable_name);
}

void VariableUsage::operator()(const WhileStatement* node) {
    node->m_condition->visit(*this);

    VariableUsage inner = with_block_scope(true);
    for (auto& statement : node->m_statements) {
        statement->visit(inner);
    }
}

VariableUsage VariableUsage::with_block_scope(bool inside_loop) noexcept {
    VariableUsage variable_usage { m_filename };
    variable_usage.m_global_symbols = m_global_symbols;
    variable_usage.m_local_symbols = m_local_symbols;
    variable_usage.m_inside_loop = inside_loop;
    return variable_usage;
}

void VariableUsage::unique_global(
        const Position& position,
        const std::string& name) noexcept {

    if (m_global_symbols.find(name) == m_global_symbols.end()) {
        m_global_symbols.insert(name);
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: duplicate symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

void VariableUsage::unique_local(
        const Position& position,
        const std::string& name) noexcept {

    if ((m_global_symbols.find(name) == m_global_symbols.end()) &&
            (m_local_symbols.find(name) == m_local_symbols.end())) {

        m_local_symbols.insert(name);
        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: duplicate symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

void VariableUsage::check_variable(
        const Position& position,
        const std::string& name) noexcept {

    if ((m_global_symbols.find(name) != m_global_symbols.end()) ||
            (m_local_symbols.find(name) != m_local_symbols.end())) {

        return;
    }

    std::cerr
        << m_filename
        << ':'
        << position
        << ": Error: unknown symbol name \""
        << name
        << "\"\n";
    std::exit(1);
}

static const char hex_compiler_header[] =
        "                            # Elf32_Ehdr: 0x08048000\n"
        "7F 45 4C 46 01 01 01 00     #     e_ident[0:7]\n"
        "00 00 00 00 00 00 00 00     #     e_ident[8:15]\n"
        "02 00                       #     e_type\n"
        "03 00                       #     e_machine\n"
        "01 00 00 00                 #     e_version\n"
        "54 80 04 08                 #     e_entry\n"
        "34 00 00 00                 #     e_phoff\n"
        "00 00 00 00                 #     e_shoff\n"
        "00 00 00 00                 #     e_flags\n"
        "34 00                       #     e_ehsize\n"
        "20 00                       #     e_phentsize\n"
        "01 00                       #     e_phnum\n"
        "28 00                       #     e_shentsize\n"
        "00 00                       #     e_shnum\n"
        "00 00                       #     e_shstrndx\n"
        "\n"
        "                            #   Elf32_Phdr: 0x08048034\n"
        "01 00 00 00                 #       p_type\n"
        "00 00 00 00                 #       p_offset\n"
        "00 80 04 08                 #       p_vaddr\n"
        "00 80 04 08                 #       p_paddr\n"
        "size                        #       p_filesz\n"
        "size                        #       p_memsz\n"
        "07 00 00 00                 #       p_flags\n"
        "00 00 00 00                 #       p_align\n"
        "                            #   _start: 0x08048054\n"
        "\n"
        "\n"
        "%add_eax_ebx:       \"01 D8\"     # add eax, ebx\n"
        "%add_eax_imm:       \"05\"        # add eax, <imm32>\n"
        "%add_esp_imm:       \"81 C4\"     # add esp, <imm32>\n"
        "%and_eax_ebx:       \"21 D8\"     # and eax, ebx\n"
        "%call_ref_eax:      \"FF 10\"     # call [eax]\n"
        "%cdq:               \"99\"        # cdq\n"
        "%cmp_eax_ebx:       \"39 D8\"     # cmp eax, ebx\n"
        "%cmp_eax_imm:       \"3D\"        # cmp eax, <imm32>\n"
        "%cmp_ebx_imm:       \"81 FB\"     # cmp ebx, <imm32>\n"
        "%idiv_ebx:          \"F7 FB\"     # idiv ebx\n"
        "%imul_eax_ebx:      \"0F AF C3\"  # imul eax, ebx\n"
        "%int_80:            \"CD 80\"     # int 0x80\n"
        "%jmp_eax:           \"FF E0\"     # jmp eax\n"
        "%mov_eax_ebp:       \"89 E8\"     # mov eax, ebp\n"
        "%mov_eax_edx:       \"89 D0\"     # mov eax, edx\n"
        "%mov_eax_imm:       \"B8\"        # mov eax, <imm32>\n"
        "%mov_eax_ref_eax:   \"8B 00\"     # mov eax, [eax]\n"
        "%mov_ebp_esp:       \"89 E5\"     # mov ebp, esp\n"
        "%mov_ebx_eax:       \"89 C3\"     # mov ebx, eax\n"
        "%mov_esp_ebp:       \"89 EC\"     # mov esp, ebp\n"
        "%mov_ref_eax_ebx:   \"89 18\"     # mov [eax], ebx\n"
        "%movzx_eax_al:      \"0F B6 C0\"  # movzx eax, al\n"
        "%movzx_ebx_bl:      \"0F B6 DB\"  # movzx ebx, bl\n"
        "%neg_eax:           \"F7 D8\"     # neg eax\n"
        "%not_eax:           \"F7 D0\"     # not eax\n"
        "%or_eax_ebx:        \"09 D8\"     # or eax, ebx\n"
        "%pop_eax:           \"58\"        # pop eax\n"
        "%pop_ebp:           \"5D\"        # pop ebp\n"
        "%pop_ebx:           \"5B\"        # pop ebx\n"
        "%pop_edx:           \"5A\"        # pop edx\n"
        "%push_eax:          \"50\"        # push eax\n"
        "%push_ebp:          \"55\"        # push ebp\n"
        "%push_ebx:          \"53\"        # push ebx\n"
        "%push_edx:          \"52\"        # push edx\n"
        "%push_imm:          \"68\"        # push <imm32>\n"
        "%ret:               \"C3\"        # ret\n"
        "%sete_al:           \"0F 94 C0\"  # sete al\n"
        "%setg_al:           \"0F 9F C0\"  # setg al\n"
        "%setge_al:          \"0F 9D C0\"  # setge al\n"
        "%setl_al:           \"0F 9C C0\"  # setl al\n"
        "%setle_al:          \"0F 9E C0\"  # setle al\n"
        "%setne_al:          \"0F 95 C0\"  # setne al\n"
        "%setne_bl:          \"0F 95 C3\"  # setne bl\n"
        "%sub_eax_ebx:       \"29 D8\"     # sub eax, ebx\n"
        "%xor_eax_ebx:       \"31 D8\"     # xor eax, ebx\n"
        "# x86 has no \"je LABEL\". Instead do \"jne l1; jmp LABEL; l1:\"\n"
        "%hop_ne:            \"75 07\"     # jne . + 0x07 => hop over mov + jmp\n"
        "\n"
        "\n";

class Compiler: public Visitor {
public:
    explicit Compiler(Writer& writer, int& next_unique_id) noexcept:
        m_writer { writer },
        m_next_unique_id { next_unique_id } {
    }

    Compiler(const Compiler&) noexcept = delete;
    Compiler& operator=(const Compiler&) noexcept = delete;

    Compiler(Compiler&&) noexcept = default;
    Compiler& operator=(Compiler&&) noexcept = default;

    ~Compiler() noexcept override = default;

    void operator()(const AddressOfExpression*) override;
    void operator()(const BinOpExpression*) override;
    void operator()(const BreakStatement*) override;
    void operator()(const CallExpression*) override;
    void operator()(const ContinueStatement*) override;
    void operator()(const ExpressionStatement*) override;
    void operator()(const ForStatement*) override;
    void operator()(const Function*) override;
    void operator()(const GlobalVar*) override;
    void operator()(const IfStatement*) override;
    void operator()(const LetStatement*) override;
    void operator()(const NumeralExpression*) override;
    void operator()(const Program*) override;
    void operator()(const ReturnStatement*) override;
    void operator()(const StringExpression*) override;
    void operator()(const UnOpExpression*) override;
    void operator()(const VariableExpression*) override;
    void operator()(const VarStatement*) override;
    void operator()(const WhileStatement*) override;

    std::string next_unique_label() noexcept {
        const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        int id = ++m_next_unique_id;
        std::string label = "";

        while (id > 0) {
            label += digits[id % 36];
            id /= 36;
        }

        while(label.size() < 3) {
            label += '0';
        }

        label += 'l';

        return std::string { label.rbegin(), label.rend() };
    }

    int next_local_offset() noexcept {
        int offset = 0;

        for (const auto& i : m_localvars) {
            offset = std::min(offset, i.second);
        }

        return offset - 4;
    }

    void address_of(const std::string& name) noexcept {
        if (m_globalvars.find(name) != m_globalvars.end()) {
            m_writer << "mov_eax_imm " << m_globalvars.at(name) << '\n';
            return;
        }

        m_writer
            << "mov_eax_ebp\n"
            << "add_eax_imm " << as_imm(m_localvars.at(name)) << '\n';
    }

    std::string byte_to_upper_hex(const unsigned char c) {
        std::string retval = "00";
        retval[0] = "0123456789ABCDEF"[0x0f & (c >> 4)];
        retval[1] = "0123456789ABCDEF"[0x0f & (c >> 0)];
        return retval;
    }

    std::string as_imm(uint32_t imm) noexcept {
        return
                byte_to_upper_hex(0xff & (imm >> 0)) +
                byte_to_upper_hex(0xff & (imm >> 8)) +
                byte_to_upper_hex(0xff & (imm >> 16)) +
                byte_to_upper_hex(0xff & (imm >> 24));
    }

    Compiler with_return_label(std::string return_label) noexcept {
        Compiler child { m_writer, m_next_unique_id };
        child.m_globalvars = m_globalvars;
        child.m_localvars = m_localvars;
        child.m_return_label = std::move(return_label);
        child.m_break_label = m_break_label;
        child.m_continue_label = m_continue_label;

        return child;
    }

    Compiler with_break_continue_label(
            std::string break_label,
            std::string continue_label) noexcept {

        Compiler child { m_writer, m_next_unique_id };
        child.m_globalvars = m_globalvars;
        child.m_localvars = m_localvars;
        child.m_return_label = m_return_label;
        child.m_break_label = std::move(break_label);
        child.m_continue_label = std::move(continue_label);

        return child;
    }

private:
    int& m_next_unique_id;
    Writer& m_writer;

    std::string m_return_label {};
    std::string m_break_label {};
    std::string m_continue_label {};

    /* variable name -> absolute label. */
    std::map<std::string, std::string> m_globalvars;

    /* maps variable name -> EBP offset. */
    std::map<std::string, int> m_localvars;
};

void compile_program(Program& program, Writer& writer) noexcept {
    int next_unique_id { 0 };
    Compiler compiler { writer, next_unique_id };
    program.visit(compiler);
}

void Compiler::operator()(const AddressOfExpression* node) {
    address_of(node->m_variable_name);
    m_writer << "push_eax\n";
}

void Compiler::operator()(const BinOpExpression* node) {
        /* save ebx */
        m_writer << "push_ebx\n";

        /* put lhs into eax, rhs into ebx */
        node->m_lhs->visit(*this);
        node->m_rhs->visit(*this);
        m_writer
            << "pop_ebx\n"
            << "pop_eax\n";

        switch (node->m_token) {
        case Token::token_plus:
            m_writer << "add_eax_ebx\n";
            break;
        case Token::token_minus:
            m_writer << "sub_eax_ebx\n";
            break;
        case Token::token_multiply:
            m_writer << "imul_eax_ebx\n";
            break;
        case Token::token_divide:
            m_writer
                << "push_edx\n"
                << "cdq\n"
                << "idiv_ebx\n"
                << "pop_edx\n";
            break;
        case Token::token_modulo:
            m_writer
                << "push_edx\n"
                << "cdq\n"
                << "idiv_ebx\n"
                << "mov_eax_edx\n"
                << "pop_edx\n";
            break;
        case Token::token_log_and:
            m_writer
                << "cmp_eax_imm 00 00 00 00\n"
                << "setne_al\n"
                << "movzx_eax_al\n"
                << "cmp_ebx_imm 00000000\n"
                << "setne_bl\n"
                << "movzx_ebx_bl\n"
                << "and_eax_ebx\n";
            break;
        case Token::token_log_or:
            m_writer
                << "cmp_eax_imm 00000000\n"
                << "setne_al\n"
                << "movzx_eax_al\n"
                << "cmp_ebx_imm 00000000\n"
                << "setne_bl\n"
                << "movzx_ebx_bl\n"
                << "or_eax_ebx\n";
            break;
        case Token::token_bit_and:
            m_writer << "and_eax_ebx\n";
            break;
        case Token::token_bit_or:
            m_writer << "or_eax_ebx\n";
            break;
        case Token::token_bit_xor:
            m_writer << "xor_eax_ebx\n";
            break;
        case Token::token_equal:
            m_writer
                << "cmp_eax_ebx\n"
                << "sete_al\n"
                << "movzx_eax_al\n";
            break;
        case Token::token_notequal:
            m_writer
                << "cmp_eax_ebx\n"
                << "setne_al\n"
                << "movzx_eax_al\n";
            break;
        case Token::token_less:
            m_writer
                << "cmp_eax_ebx\n"
                << "setl_al\n"
                << "movzx_eax_al\n";
            break;
        case Token::token_lessequal:
            m_writer
                << "cmp_eax_ebx\n"
                << "setle_al\n"
                << "movzx_eax_al\n";
            break;
        case Token::token_greater:
            m_writer
                << "cmp_eax_ebx\n"
                << "setg_al\n"
                << "movzx_eax_al\n";
            break;
        case Token::token_greaterequal:
            m_writer
                << "cmp_eax_ebx\n"
                << "setge_al\n"
                << "movzx_eax_al\n";
            break;
        }

        /* restore ebx */
        m_writer << "pop_ebx\n";

        m_writer << "push_eax\n";
}

void Compiler::operator()(const BreakStatement* /* node */) {
    m_writer << "mov_eax_imm " << m_break_label << "\n";
    m_writer << "jmp_eax\n";
}

void Compiler::operator()(const CallExpression* node) {
    /* put arguments on the stack, right to left */
    for (
            auto it = node->m_arguments.rbegin();
            it != node->m_arguments.rend();
            ++it) {
        (*it)->visit(*this);
    }

    /* call function */
    address_of(node->m_variable_name);
    m_writer << "call_ref_eax\n";

    /* clean up stack */
    m_writer << "add_esp_imm " << as_imm(node->m_arguments.size() * 4) << "\n";

    /* return value */
    m_writer << "push_eax\n";
}

void Compiler::operator()(const ContinueStatement* node) {
    m_writer << "mov_eax_imm " << m_continue_label << "\n";
    m_writer << "jmp_eax\n";
}

void Compiler::operator()(const ExpressionStatement* node) {
    /* calculate expression */
    node->m_expression->visit(*this);

    /* discard result */
    m_writer << "pop_eax\n";
}

void Compiler::operator()(const ForStatement* node) {
    const std::string for_begin = next_unique_label();
    const std::string for_continue = next_unique_label();
    const std::string for_end = next_unique_label();

    Compiler inner = with_break_continue_label(for_end, for_continue);

    /* setup and initialize loop variable */
    inner.m_localvars[node->m_variable_name] = next_local_offset();
    m_writer
        << "push_imm 00000000\n"
        << "push_ebx\n";
    node->m_initial->visit(*this);
    m_writer << "pop_ebx\n";
    inner.address_of(node->m_variable_name);
    m_writer
        << "mov_ref_eax_ebx\n"
        << "pop_ebx\n";

    /* condition */
    m_writer << '.' << for_begin << ":\n";
    node->m_condition->visit(inner);
    m_writer
        << "pop_eax\n"
        << "cmp_eax_imm 00 00 00 00\n"
        << "hop_ne\n"
        << "mov_eax_imm " << for_end << '\n'
        << "jmp_eax\n";

    /* loop body */
    for (auto& i : node->m_statements) {
        i->visit(inner);
    }

    /* update */
    m_writer
        << '.' << for_continue << ":\n"
        << "push_ebx\n";
    node->m_update->visit(inner);
    m_writer << "pop_ebx\n";
    inner.address_of(node->m_variable_name);
    m_writer
        << "mov_ref_eax_ebx\n"
        << "pop_ebx\n";

    /* loop back */
    m_writer
        << "mov_eax_imm " << for_begin << '\n'
        << "jmp_eax\n";

    /* remove loop variable */
    m_writer
        << '.' << for_end << ":\n"
        << "pop_eax\n";
}

void Compiler::operator()(const Function* node) {
    const std::string fun_begin = next_unique_label();
    const std::string fun_end = next_unique_label();
    const std::string fun_entry = next_unique_label();
    const std::string fun_return = next_unique_label();
    m_globalvars[node->m_name] = fun_begin;

    /* register arguments as local variables */
    m_localvars.clear();
    for (int i = 0; i < static_cast<int>(node->m_arguments.size()); ++i) {
        m_localvars[node->m_arguments[i]] = 8 + 4 * i;
    }

    m_writer
        << "\n"
        << "##\n"
        << "## Function \"" << node->m_name << "\"\n"
        << "##\n"
        << "\n"
        << "mov_eax_imm " << fun_end << '\n'
        << "jmp_eax\n"
        << '.' << fun_begin << ":\n"
        << "00 00 00 00\n"
        << '.' << fun_entry << ":\n"
        << "push_ebp\n"
        << "mov_ebp_esp\n";

    Compiler inner = with_return_label(fun_return);
    for (auto& statement : node->m_statements) {
        statement->visit(inner);
    }

    m_writer
        /* set up default return value. */
        << "push_imm 00 00 00 00\n"
        /* "return" statements jumps here. expects return value on stack. */
        << '.' << fun_return << ":\n"
        << "pop_eax\n"
        /* tear down stack frame. */
        << "mov_esp_ebp\n"
        << "pop_ebp\n"
        /* leave function. */
        << "ret\n";

    m_writer
        /* initialize function ptr variable */
        << '.' << fun_end << ":\n"
        << "mov_eax_imm " << fun_entry << '\n'
        << "mov_ebx_eax\n"
        << "mov_eax_imm " << fun_begin << '\n'
        << "mov_ref_eax_ebx\n";
}

void Compiler::operator()(const GlobalVar* node) {
    const std::string var_begin = next_unique_label();
    const std::string var_end = next_unique_label();
    m_globalvars[node->m_name] = var_begin;

    m_writer
        << "\n"
        << "##\n"
        << "## GlobalVar \"" << node->m_name << "\"\n"
        << "##\n"
        << "\n";

    /* define and jump over memory location where variable is stored */
    m_writer
        << "mov_eax_imm " << var_end << '\n'
        << "jmp_eax\n"
        << '.' << var_begin << ":\n"
        << "00 00 00 00\n"
        << '.' << var_end << ":\n";

    /* push initial value to the stack */
    node->m_value->visit(*this);

    /* store value */
    m_writer
        << "pop_ebx\n"
        << "mov_eax_imm " << var_begin << '\n'
        << "mov_ref_eax_ebx\n";
}

void Compiler::operator()(const IfStatement* node) {
    const std::string else_begin = next_unique_label();
    const std::string if_end = next_unique_label();

    node->m_condition->visit(*this);

    m_writer
        << "pop_eax\n"
        << "cmp_eax_imm 00 00 00 00\n"
        << "hop_ne\n"
        << "mov_eax_imm " << else_begin << '\n'
        << "jmp_eax\n";

    Compiler inner_then = with_return_label(m_return_label);
    for (auto& statement : node->m_then_statements) {
        statement->visit(inner_then);
    }

    m_writer
        << "mov_eax_imm " << if_end << '\n'
        << "jmp_eax\n"
        << '.' << else_begin << ":\n";

    Compiler inner_else = with_return_label(m_return_label);
    for (auto& statement : node->m_else_statements) {
        statement->visit(inner_else);
    }

    m_writer << '.' << if_end << ":\n";
}

void Compiler::operator()(const LetStatement* node) {
    /* save ebx */
    m_writer << "push_ebx\n";

    /* put value into ebx */
    node->m_expression->visit(*this);
    m_writer << "pop_ebx\n";

    /* put target address into eax */
    address_of(node->m_variable_name);

    /* store */
    m_writer << "mov_ref_eax_ebx\n";

    /* restore ebx */
    m_writer << "pop_ebx\n";
}

void Compiler::operator()(const NumeralExpression* node) {
    m_writer << "push_imm " << as_imm(node->m_value) << '\n';
}

void Compiler::operator()(const Program* node) {
    m_writer << hex_compiler_header;

    for (auto& i : node->m_globalvars) {
        i.visit(*this);
    }

    for (auto& i : node->m_functions) {
        i.visit(*this);
    }

    m_writer
        << "\n"
        << "# Call main\n"
        << "mov_eax_imm " << m_globalvars["main"] << "\n"
        << "call_ref_eax\n"
        << "\n"
        << "# Terminate\n"
        << "mov_ebx_eax\n"
        << "mov_eax_imm 01 00 00 00\n"
        << "int_80\n";
}

void Compiler::operator()(const ReturnStatement* node) {
    node->m_expression->visit(*this);

    m_writer
        << "mov_eax_imm " << m_return_label << '\n'
        << "jmp_eax\n";
}

void Compiler::operator()(const StringExpression* node) {
    const std::string data_begin = next_unique_label();
    const std::string data_end = next_unique_label();

    /* jump over data */
    m_writer
        << "mov_eax_imm " << data_end << '\n'
        << "jmp_eax\n"
        << '.' << data_begin << ":\n";

    /* emit string data (null terminated) */
    for (const auto c : node->m_value) {
        m_writer << byte_to_upper_hex(0xff & c) << ' ';
    }
    m_writer << "00\n";

    /* store address */
    m_writer
        << '.' << data_end << ":\n"
        << "push_imm " << data_begin << '\n';
}

void Compiler::operator()(const UnOpExpression* node) {
    node->m_rhs->visit(*this);
    m_writer << "pop_eax\n";

    if (node->m_token == Token::token_minus) {
        m_writer << "neg_eax\n";
    }

    if (node->m_token == Token::token_bit_not) {
        m_writer << "not_eax\n";
    }

    if (node->m_token == Token::token_log_not) {
        m_writer
        << "cmp_eax_imm 00 00 00 00\n"
        << "sete_al\n"
        << "movzx_eax_al\n";
    }

    m_writer << "push_eax\n";
}

void Compiler::operator()(const VariableExpression* node) {
    address_of(node->m_variable_name);
    m_writer
        << "mov_eax_ref_eax\n"
        << "push_eax\n";
}

void Compiler::operator()(const VarStatement* node) {
    m_localvars[node->m_variable_name] = next_local_offset();
    m_writer << "push_imm 00 00 00 00\n";

    /* save ebx */
    m_writer << "push_ebx\n";

    node->m_expression->visit(*this);

    /* value in ebx */
    m_writer << "pop_ebx\n";

    /* address in eax */
    address_of(node->m_variable_name);

    /* store */
    m_writer << "mov_ref_eax_ebx\n";

    /* restore ebx */
    m_writer << "pop_ebx\n";
}

void Compiler::operator()(const WhileStatement* node) {
    const std::string while_begin = next_unique_label();
    const std::string while_end = next_unique_label();

    m_writer << '.' << while_begin << ":\n";

    node->m_condition->visit(*this);

    m_writer
        << "pop_eax\n"
        << "cmp_eax_imm 00 00 00 00\n"
        << "hop_ne\n"
        << "mov_eax_imm " << while_end << '\n'
        << "jmp_eax\n";

    Compiler inner = with_break_continue_label(while_end, while_begin);
    for (auto& i : node->m_statements) {
        i->visit(inner);
    }

    m_writer
        << "mov_eax_imm " << while_begin << '\n'
        << "jmp_eax\n"
        << '.' << while_end << ":\n";
}

} /* namespace arabilis */
