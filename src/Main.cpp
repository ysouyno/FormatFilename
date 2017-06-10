#include "FormatFilename.h"

#if defined(_MSC_VER)
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  if (0 == argv[1]) {
    std::cout << "Usage: FormatFilename FILE" << std::endl;
    return 1;
  }

#if defined(_MSC_VER)
  std::wstring path_file(argv[1]);
#else
  std::string input_file(argv[1]);
  std::wstring path_file = s2ws(input_file);
#endif

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
