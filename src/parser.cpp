#include "parser.hpp"

Parser::Parser(const std::vector<token>& tokens) : tokens(tokens), current(0) {
}

bool Parser::parse() {
  return Program();
}

token Parser::currToken() {
  return tokens[current];
}

token Parser::nextToken() {
  return tokens[current++];
}

bool Parser::nonTerminal(std::string name) {
  if (currToken().token_name == name) {
    nextToken();
    return true;
  }
  return false;
}

/*
  Type' -> [ ] Type'
  Type' -> ε
*/
bool Parser::TypePrime() {
  if (nonTerminal("TOKEN_[") && nonTerminal("TOKEN_]") && TypePrime()) {
    return true;
  }
  return true;
}

/*
  BasicType -> IntType
  BasicType -> BoolType
  BasicType -> CharType
  BasicType -> StringType
  BasicType -> VoidType
*/
bool Parser::BasicType() {
  if (nonTerminal("TOKEN_IntType") || nonTerminal("TOKEN_BoolType") || 
      nonTerminal("TOKEN_CharType") || nonTerminal("TOKEN_StringType") || 
      nonTerminal("TOKEN_VoidType")) {
    return true;
  }
  return false;
}

/*
  Type->BasicType Type'
*/
bool Parser::Type() {
  if (BasicType() && TypePrime()) {
    return true;
  }
  return false;
}

/*
  Function -> Type Identifier (Params) { StmtList }
*/
bool Parser::Function() {
  if (Type() && 
      nonTerminal("TOKEN_ID") && 
      nonTerminal("TOKEN_(") && 
      Params() && 
      nonTerminal("TOKEN_)") && 
      CompoundStmt()) {
    return true;
  }
  return false;
}

/*
  VarDecl' -> ;
  VarDecl' -> = Expression ;
*/
bool Parser::VarDeclPrime() {
  if (nonTerminal("TOKEN_;")) {
    return true;
  }
  else if (nonTerminal("TOKEN_=") && Expression() && nonTerminal("TOKEN_;")) {
    return true;
  }
  return false;
}

/*
  VarDecl -> Type Identifier VarDecl'
*/
bool Parser::VarDecl() {
  if (Type() && nonTerminal("TOKEN_ID") && VarDeclPrime()) {
    return true;
  }
  return false;
}

/*
  Declaration -> [ Function ]
  Declaration -> VarDecl
*/
bool Parser::Declaration() {
  if (nonTerminal("TOKEN_[") && Function() && nonTerminal("TOKEN_]")) {
    return true;
  }
  else if (VarDecl()) {
    return true;
  }
  return false;
}

/*
  Program' -> Declaration Program'
  Program' -> ε
*/
bool Parser::ProgramPrime() {
    if (current >= tokens.size()) {
        return true; 
    }
    if (Declaration() && ProgramPrime()) {
        return true;
    } else { return false;}
    return true;
}

/*
  Program -> Declaration Program'
*/
bool Parser::Program() {
  if (Declaration() && ProgramPrime()) {
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////

/*
  Params -> ParamList
  Params -> ε
*/
bool Parser::Params() {
  if (ParamList()) {
    return true;
  }
  return true;
}

/*
  ParamList -> Type Identifier ParamList'
*/
bool Parser::ParamList() {
  if (Type() && nonTerminal("TOKEN_ID") && ParamListPrime()) {
    return true;
  }
  return false;
}

/*
  ParamList' -> , Type Identifier ParamList'
  ParamList' -> ε
*/
bool Parser::ParamListPrime() {
  if (nonTerminal("TOKEN_,") && Type() && nonTerminal("TOKEN_ID") && ParamListPrime()) {
    return true;
  }
  return true;
}

/*
  StmtList -> Statement StmtList'
*/
bool Parser::StmtList() {
  if (Statement() && StmtListPrime()) {
    return true;
  }
  return false;
}

/*
  StmtList' -> Statement StmtList'
  StmtList' -> ε
*/
bool Parser::StmtListPrime() {
  if (Statement() && StmtListPrime()) {
    return true;
  }
  return true;
}

/*
  CompoundStmt -> { StmtList }
*/
bool Parser::CompoundStmt() {
  if (nonTerminal("TOKEN_{") && StmtList() && nonTerminal("TOKEN_}")) {
    return true;
  }
  return false;
}

/*
ExprStmt ::= Expression ;
ExprStmt ::= ;
*/
bool Parser::ExprStmt(){
  if(Expression()&& nonTerminal("TOKEN_;")){
    return true;
  }
  else if(nonTerminal("TOKEN_;")){
    return true;
  }
  return false;
}

/*
PrintStmt -> print ( ExprList ) ;
*/
bool Parser::PrintStmt(){
  if (nonTerminal("TOKEN_print")&&
      nonTerminal("TOKEN_(")&&
      ExprList()&&
      nonTerminal("TOKEN_)")&&
      nonTerminal("TOKEN_;")){
    return true;
  }
  return false;
}

/*
ReturnStmt -> return Expression ;
*/
bool Parser::ReturnStmt(){
  if (nonTerminal("TOKEN_return")&&
      Expression()&& 
      nonTerminal("TOKEN_;")){
    return true;
  }
  return false;
}

/*
ForStmt -> for ( ExprStmt Expression ; ) Statement
*/
bool Parser::ForStmt() {
    if (nonTerminal("TOKEN_for") &&
        nonTerminal("TOKEN_(") &&
        ExprStmt() &&
        ExprStmt() &&
        ExprStmt() &&
        nonTerminal("TOKEN_)") &&
        Statement()) {
        return true;
    }
    return false;
}
/*
AuxIf -> else {Statement}
AuxIf -> ‘’
*/
bool Parser::AuxIf() {
    if (nonTerminal("TOKEN_else") &&
        nonTerminal("TOKEN_{") &&
        StmtList() &&
        nonTerminal("TOKEN_}")) {
        return true;
    }
    return true;
}

/*
IfStmt -> if ( Expression ) {Statement} AuxIf
*/
bool Parser::IfStmt() {
    if (nonTerminal("TOKEN_if") &&
        nonTerminal("TOKEN_(") &&
        Expression() &&
        nonTerminal("TOKEN_)") &&
        nonTerminal("TOKEN_{") &&
        StmtList() &&
        nonTerminal("TOKEN_}") &&
        AuxIf()) {
        return true;
    }
    return false;
}

/*
  Statement -> VarDecl
  Statement -> IfStmt
  Statement -> ForStmt
  Statement -> ReturnStmt
  Statement -> ExprStmt
  Statement -> PrintStmt
  Statement -> { StmtList }
  Statement -> PrintStmt
*/
bool Parser::Statement() {
  return VarDecl() || IfStmt() || ForStmt() || ReturnStmt() || ExprStmt() || CompoundStmt() || PrintStmt();
}

/*
auxPrimary ::= ( ExprList )
auxPrimary ::= ''
*/
bool Parser::AuxPrimary() {
    if (nonTerminal("TOKEN_(") &&
        ExprList() &&
        nonTerminal("TOKEN_)")) {
        return true;
    }
    return true;
}


/*
Primary ::= Identifier auxPrimary
Primary ::= IntegerLiteral -->num
Primary ::= CharLiteral  --> 'char'
Primary ::= StringLiteral --> "text_string"
Primary ::= BooleanLiteral --> True o false
Primary ::= ( Expression )
*/

bool Parser::Primary(){
  if(nonTerminal("TOKEN_ID") && AuxPrimary()){
    return true;
  }
  if(nonTerminal("TOKEN_Num")){
    return true;
  }
  if(nonTerminal("TOKEN_True")||nonTerminal("TOKEN_False")){
    return true;
  }
  if (nonTerminal("TOKEN_Comilla_doble") &&
      nonTerminal("TOKEN_Text_string") &&
      nonTerminal("TOKEN_Comilla_doble")){
    return true;
  }
  if (nonTerminal("TOKEN_Comilla") &&
      nonTerminal("TOKEN_ID")&&
      nonTerminal("TOKEN_Comilla")){
    return true;
  }
  if (nonTerminal("TOKEN_(")  &&
      Expression() && 
      nonTerminal("TOKEN_)")){
    return true;
  }
  return false;
}

/*
Factor' ::= [ Expression ] Factor'
Factor' ::= ''
*/
bool Parser::FactorPrime(){
  if (nonTerminal("TOKEN_[")&&
      Expression()&&
      nonTerminal("TOKEN_]")&&
      FactorPrime()){
    return true;
  }
  return true;
}

/*
Factor ::= Primary Factor'
*/
bool Parser::Factor(){
  if(Primary() && FactorPrime()){
    return true;
  }
  return false;
}

/*
Unary ::= ! Unary
Unary ::= - Unary
Unary ::= Factor
*/
bool Parser::Unary(){
  if (nonTerminal("TOKEN_!") || nonTerminal("TOKEN_-") || Factor()) {
    return true;
  }
  return false;
}

/*
Term' ::= * Unary Term'
Term' ::= / Unary Term'
Term' ::= % Unary Term'
Term' ::= ''
*/
bool Parser::TermPrime(){
  if ((nonTerminal("TOKEN_*") || nonTerminal("TOKEN_/") || nonTerminal("TOKEN_%")) &&
      Unary() &&
      TermPrime()){
    return true;
  }
  return true;
}

/*
Term ::= Unary Term'
*/
bool Parser::Term(){
  if(Unary() && TermPrime()){
    return true;
  }
  return false;
}

/*
Expr' ::= + Term Expr'
Expr' ::= - Term Expr'
Expr' ::= ''
*/
bool Parser::ExprPrime(){
  if ((nonTerminal("TOKEN_+") || nonTerminal("TOKEN_-") ) &&
      Term()&&
      ExprPrime()){
    return true;
  }
  return true;
}

/*
Expr ::= Term Expr'
*/
bool Parser::Expr(){
  if(Term() && ExprPrime()){
    return true;
  }
  return false;
}

/*
RelExpr' ::= < Expr RelExpr'
RelExpr' ::= > Expr RelExpr'
RelExpr' ::= <= Expr RelExpr'
RelExpr' ::= >= Expr RelExpr'
RelExpr' ::= ''
*/
bool Parser::RelExprPrime(){
  if ((nonTerminal("TOKEN_<") || nonTerminal("TOKEN_>") || nonTerminal("TOKEN_<=")||nonTerminal("TOKEN_>=")) &&
      Expr()&&
      RelExprPrime()){
    return true;
  }
  return true;
}

/*
RelExpr ::= Expr RelExpr'
*/
bool Parser::RelExpr(){
  if(Expr()&&RelExprPrime()){
    return true;
  }
  return false;
}

/*
EqExpr' ::= == RelExpr EqExpr'
EqExpr' ::= != RelExpr EqExpr'
EqExpr' ::= ''
*/
bool Parser::EqExprprime(){
  if ((nonTerminal("TOKEN_==") || nonTerminal("TOKEN_!=")) &&
      RelExpr()&&
      EqExprprime()){
    return true;
  }
  return true;
}

/*
EqExpr ::= RelExpr EqExpr'
*/
bool Parser::EqExpr(){
  if(RelExpr() && EqExprprime()){
    return true;
  }
  return false;
}

/*
AndExpr' ::= && EqExpr AndExpr'
AndExpr' ::= ''
*/
bool Parser:: AndExprPrime(){
  if (nonTerminal("TOKEN_&&")&&
      EqExpr()&&
      AndExprPrime()){
    return true;
  }
  return true;
}

/*
AndExpr ::= EqExpr AndExpr'
*/
bool Parser::AndExpr(){
  if(EqExpr()&&AndExprPrime()){
    return true;
  }
  return false;
}

/*
OrExpr' ::= || AndExpr OrExpr'
OrExpr' ::= ''
*/
bool Parser::OrExprPrime(){
  if (nonTerminal("TOKEN_||")&&
      AndExpr()&&
      OrExprPrime()){
    return true;
  }
  return true;
}

/*
OrExpr ::= AndExpr OrExpr'
*/
bool Parser::OrExpr(){
  if (AndExpr()&& OrExprPrime()){
    return true;
  }
  return false;
}

/*
AuxExpression ::= = Expression
AuxExpression ::= ''
*/
bool Parser::AuxExpression(){
  if (nonTerminal("TOKEN_=") && Expression()){
    return true;
  }
  return true;
}

/*
Expression ::= OrExpr AuxExpression
*/

bool Parser::Expression(){
  if (OrExpr()&&AuxExpression()){
    return true;
  }
  return false;
}

/*
ExprList' ::= , Expression ExprList'
ExprList' ::= ''
*/
bool Parser::ExprListPrime(){
  if (Expression()&& ExprListPrime()){
    return true;
  }
  return true;
}

/*
ExprList ::= Expression ExprList'
*/
bool Parser::ExprList(){
  if (Expression()&& ExprListPrime()){
    return true;
  }
  return false;
}
