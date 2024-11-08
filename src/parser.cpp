#include "parser.hpp"
#include "iostream"
#include <unordered_set>
int errorCount = 0;
string fallo = "vacio";
int token_num = 0;
Parser::Parser(const std::vector<token>& tokens) : tokens(tokens), current(0) {
}

bool Parser::parse() {
    bool success = Program();
    if (errorCount > 0) {
        std::cerr << "Parsing completed with " << errorCount << " errors." << std::endl;
        return false;
    }
    return success;
}

token Parser::currToken() {
  return tokens[current];
}

token Parser::nextToken() {
  return tokens[current++];
}

void Parser::fail(string message) {
   // std::cerr << "Error at token " << current << ": " << message << std::endl;
    if (fallo == "vacio") { fallo = message; token_num = current;}
    
}

void Parser::syncToDelimiter() {
    static const unordered_set<string> delimiters = { "TOKEN_;", "TOKEN_]", "TOKEN_)" };

    while (current < tokens.size() && delimiters.find(currToken().token_name) == delimiters.end()) {
        nextToken();
    }

    if (current < tokens.size()) {
        nextToken();
    }
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
  fail("Tipo básico no valido");
  return false ;
}

/*
  Type->BasicType Type'
*/
bool Parser::Type() {
  if (BasicType() && TypePrime()) {
    return true;
  }
  fail("No es un tipo valido");
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
  fail("Función mal declarada");
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
  fail("Error en la declaracion de variable");
  return false;
}

/*
  VarDecl -> Type Identifier VarDecl'
*/
bool Parser::VarDecl() {
  if (Type() && nonTerminal("TOKEN_ID") && VarDeclPrime()) {
    return true;
  }
  fail("Error en declaracion de variable: se esperaba un identificador o un punto y coma");
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
  fail("Error en declaracion: se esperaba una función o declaracion de variable");
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
    }
    else {
        fail("Failed to parse Declaration");
        errorCount++;
        cerr << "Error at token " << token_num << ": " << fallo << endl;
        syncToDelimiter();  // Sincroniza con el siguiente delimitador
        fallo = "vacio";
        return ProgramPrime();
    }
    return true;
}

/*
  Program -> Declaration Program'
*/
bool Parser::Program() {
  if (Declaration() && ProgramPrime()) {
    return true;
  }
  fail("Error en el programa: inicio no valido");
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
  fail("Error en lista de parámetros: se esperaba tipo e identificador");
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
  fail("Error en lista de sentencias: sentencia no válida");
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
    fail("Error en bloque de sentencias: se esperaba '{' o '}'");
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
  fail("Error en sentencia de expresion: falta punto y coma");
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
  fail("Error en sentencia de impresión: se esperaba '(' o ')'");
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
  fail("Error en sentencia de retorno: falta expresion o punto y coma");
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
    fail("Error en sentencia for: se esperaba ';' o ')'");
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
    fail("Error en sentencia if: se esperaba '(' o '{'");
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
bool Parser::AuxPrimary(){
  if (nonTerminal("TOKEN_(")  &&
      ExprList() && 
      nonTerminal("TOKEN_)")){
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
  fail("Error en Primary: se esperaba un identificador, número, booleano, cadena de texto, o una expresion entre paréntesis.");
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
  fail("Error en Factor: se esperaba una expresion primaria seguida de un operador o índice opcional.");
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
  fail("Error en Unary: se esperaba un operador de negación '!' o '-' o una expresion de factor.");
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
  fail("Error en Term: se esperaba una expresion unaria seguida de un operador de multiplicación, división o módulo.");
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
  fail("Error en Expr: se esperaba un término seguido de un operador de suma o resta.");
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
  fail("Error en RelExpr: se esperaba una expresion relacional con operadores de comparación (<, >, <=, >=).");
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
  fail("Error en EqExpr: se esperaba una expresion de igualdad con operadores '==' o '!='.");
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
  fail("Error en AndExpr: se esperaba una expresion AND con el operador '&&'.");
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
  fail("Error en OrExpr: se esperaba una expresion OR con el operador '||'.");
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
  fail("Error en Expression: se esperaba una expresion OR seguida de una posible asignación.");
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
  fail("Error en ExprList: se esperaba una expresion en la lista de expresiones.");
  return false;
}
