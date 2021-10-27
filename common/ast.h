#pragma once

#include <ostream>
#include <functional>

#include "lexer.h"
#include "colors.h"

namespace tiny {

    class ASTBase;

    /** A simple class for pretty printing ASTs.

        Behaves similarly to a stream, but supports more advanced features, such as identation and syntax coloring.
     */
    class ASTPrettyPrinter {
    public:

        using Manipulator = std::function<ASTPrettyPrinter &(ASTPrettyPrinter &)>;

        ASTPrettyPrinter(std::ostream & s):
            s_{s} {
        }

        color identifier = color::lightGreen;
        color keyword = color::green;
        color symbol = color::lightBlue;
        color numberLiteral = color::lightRed;
        color charLiteral = color::lightMagenta;
        color stringLiteral = color::lightMagenta;
        color comment = color::gray;
        color type = color::cyan;

        void indent() {
            ++indent_;
        }

        void dedent() {
            assert(indent_ > 0);
            --indent_;
        }

        void newline() {
            s_ << std::endl;
            for (size_t i = 0; i < indent_; ++i)
                s_ << tab_;

        }

        ASTPrettyPrinter & operator << (color what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (char const * what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (std::string const & what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (int what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (int64_t what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (size_t what) {
            s_ << what;
            return *this;
        }

        ASTPrettyPrinter & operator << (double what) {
            s_ << what;
            return *this;
        }




    private:
        size_t indent_ = 0;
        std::string tab_ = "    ";
        std::ostream & s_;


    };

    /** Base class for AST nodes.

        Serves as a base class for the

     */
    class ASTBase {
        friend class ASTPrettyPrinter;
    public:
        virtual ~ASTBase() = default;

        ASTBase(Token const & t):
            l_{t.location()} {
        }

        SourceLocation location() const {
            return l_;
        }

        virtual void print(ASTPrettyPrinter & p) const = 0;

    protected:

    private:
        SourceLocation l_;

    };

    inline ASTPrettyPrinter & operator << (ASTPrettyPrinter & p, ASTBase const & what) {
        what.print(p);
        return p;
    }

}