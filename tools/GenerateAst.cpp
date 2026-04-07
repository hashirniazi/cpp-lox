#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// --- NEW: Generates the Visitor interface! ---
void defineVisitor(std::ofstream& file, const std::string& baseName, const std::vector<std::string>& types) {
    // 1. Forward declare all AST nodes so the Visitor can reference them
    for (const std::string& type : types) {
        std::string className = split(type, '-')[0];
        file << "struct " << className << ";\n";
    }
    file << "\n";

    // 2. Define the Visitor interface
    file << "struct Visitor {\n";
    file << "    virtual ~Visitor() = default;\n";
    for (const std::string& type : types) {
        std::string className = split(type, '-')[0];
        // e.g., virtual std::any visitBinary(Binary& expr) = 0;
        file << "    virtual std::any visit" << className << "(" << className << "& expr) = 0;\n";
    }
    file << "};\n\n";
}

void defineType(std::ofstream& file, const std::string& baseName, const std::string& className, const std::string& fieldList) {
    file << "struct " << className << " : public " << baseName << " {\n";
    file << "    " << className << "(" << fieldList << ")\n";
    file << "      : ";
    
    std::vector<std::string> fields = split(fieldList, ',');
    for (size_t i = 0; i < fields.size(); ++i) {
        std::vector<std::string> nameParts = split(fields[i], ' ');
        std::string name = nameParts.back(); 
        file << name << "(std::move(" << name << "))";
        if (i < fields.size() - 1) file << ", ";
    }
    file << " {}\n\n"; 

    // --- NEW: Add the accept() method to the subclass ---
    file << "    std::any accept(Visitor& visitor) override {\n";
    file << "        return visitor.visit" << className << "(*this);\n";
    file << "    }\n\n";

    for (const std::string& field : fields) {
        file << "    " << field << ";\n";
    }
    file << "};\n\n";
}

void defineAst(const std::string& outputDir, const std::string& baseName, const std::vector<std::string>& types) {
    std::string path = outputDir + "/" + baseName + ".hpp";
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << "\n";
        exit(1);
    }

    file << "#ifndef " << baseName << "_HPP\n";
    file << "#define " << baseName << "_HPP\n\n";
    
    // --- NEW: Included <any> for our generic return types ---
    file << "#include <any>\n";
    file << "#include <memory>\n";
    file << "#include <vector>\n";
    file << "#include \"Token.hpp\"\n\n";

    // --- NEW: Generate the Visitor interface before the base Expr class ---
    defineVisitor(file, baseName, types);

    file << "struct " << baseName << " {\n";
    file << "    virtual ~" << baseName << "() = default;\n";
    
    // --- NEW: Add the pure virtual accept() method to the base class ---
    file << "    virtual std::any accept(Visitor& visitor) = 0;\n";
    
    file << "};\n\n";

    for (const std::string& type : types) {
        std::vector<std::string> parts = split(type, '-');
        std::string className = parts[0];
        std::string fields = parts[1];
        defineType(file, baseName, className, fields);
    }

    file << "#endif\n";
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: generate_ast <output directory>\n";
        return 64;
    }

    std::string outputDir = argv[1];

    std::vector<std::string> types = {
        "Binary   - std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right",
        "Grouping - std::unique_ptr<Expr> expression",
        "Literal  - LiteralValue value",
        "Unary    - Token op, std::unique_ptr<Expr> right"
    };

    defineAst(outputDir, "Expr", types);

    return 0;
}