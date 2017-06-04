#include "FormatFilename.h"

int main(int argc, char *argv[])
{
  if (0 == argv[1]) {
    std::cout << "Usage: FormatFilename FILE" << std::endl;
    return 1;
  }

  std::string input_file(argv[1]);
  std::wstring path_file = s2ws(input_file);

  if (!file_is_exists(path_file)) {
    std::cout << "\"" << path_file.c_str() << "\" not exists" << std::endl;
    return 1;
  }

  if (file_is_directory(path_file)) {
    std::cout << "Directory mode" << std::endl;
    rename_dirs_and_files(path_file);
  }
  else {
    std::cout << "Single file mode" << std::endl;
    rename_file(path_file);
  }

  return 0;
}
