#include <iostream>
#include "parser.hpp"
#include "scanner.hpp"

using namespace std;

int main() {

  string filename = "../code.txt";
  read_file(filename, text_Arr, sizee);

  scanner(text_Arr);
  for (auto it = tokens.begin(); it != tokens.end(); ) {
    if (it->token_name == "TOKEN_COMLINE" || 
        it->token_name == "TOKEN_COMBLOCKS" || 
        it->token_name == "TOKEN_COMBLOCKE") {
        it = tokens.erase(it);  // Erase and get the next valid iterator
    } else {
        ++it;  // Only increment if no erase happened
    }
  }
  for (auto tok : tokens) {
    cout << tok.token_name << endl;
  }
    // Crear el objeto Parser con el vector de tokens
    Parser parser(tokens);

    // Ejecutar el análisis sintáctico
    if (parser.parse()) {
        std::cout << "Parsing exitoso." << std::endl;
    }
    else {
        std::cout << "Error de parsing." << std::endl;
    }

  return 0;
}