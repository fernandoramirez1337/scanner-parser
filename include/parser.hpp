#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

using namespace std;

struct token {
    string token_name;
    string text;
    bool errorCount;
    int line;
    int col;
    token(string N_, string T_, int L_, int C_) { token_name = N_; text = T_; line = L_; col = C_; }
    token(string N_) : token_name(N_), text("test") {}
};

class Parser {
public:
  Parser(const std::vector<token>& tokens);
  bool parse();

private:
  std::vector<token> tokens;
  size_t current;

  token currToken();
  void syncToDelimiter();
  token nextToken();
  void fail(string message);
  bool nonTerminal(std::string name);
  bool Program();
  bool ProgramPrime();
  bool Declaration();
  bool VarDecl();
  bool VarDeclPrime();
  bool Type();
  bool TypePrime();
  bool BasicType();
  bool Function();
  bool Params();
  bool ParamList();
  bool ParamListPrime();
  bool CompoundStmt();
  bool StmtList();
  bool StmtListPrime();
  bool Statement();
  bool IfStmt();
  bool ForStmt();
  bool AuxIf();
  bool ReturnStmt();
  bool PrintStmt();
  bool ExprStmt();
  bool ExprList();
  bool ExprListPrime();
  bool Expression();
  bool AuxExpression();
  bool OrExpr();
  bool OrExprPrime();
  bool AndExpr();
  bool AndExprPrime();
  bool EqExpr();
  bool EqExprprime();
  bool RelExpr();
  bool RelExprPrime();
  bool Expr();
  bool ExprPrime();
  bool Term();
  bool TermPrime();
  bool Unary();
  bool Factor();
  bool FactorPrime();
  bool Primary();
  bool AuxPrimary();
};

#endif // PARSER_HPP
