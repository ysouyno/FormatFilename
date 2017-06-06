#include "FormatFilename.h"

std::wstring s2ws(const std::string & str)
{
  if (str.empty()) {
    return L"";
  }

  setlocale(LC_CTYPE, "en_US.UTF-8");
  size_t len = str.size() + 1;
  wchar_t *p = new wchar_t[len];
  mbstowcs(p, str.c_str(), len);
  std::wstring w_str(p);
  delete[]p;

  return w_str;
}

std::string ws2s(const std::wstring & w_str)
{
  if (w_str.empty()) {
    return "";
  }

  setlocale(LC_CTYPE, "en_US.UTF-8");
  size_t len = w_str.size() * 4 + 1;
  char *p = new char[len];
  wcstombs(p, w_str.c_str(), len);
  std::string str(p);
  delete[]p;

  return str;
}

bool file_is_exists(const std::wstring & path_file)
{
#if defined(_MSC_VER)
#if _MSC_VER >= 1900
  return std::tr2::sys::exists(path_file);
#else
  if (PathFileExistsW(path_file.c_str()))
    return true;
  else
    return false;
#endif
#else
  std::string tmp = ws2s(path_file);
  struct stat st;
  return (0 == stat(tmp.c_str(), &st));
#endif
}

bool file_is_directory(const std::wstring & path_file)
{
#if defined(_MSC_VER)
#if _MSC_VER >= 1900
  return std::tr2::sys::is_directory(path_file);
#else
  if (PathIsDirectoryW(path_file.c_str()))
    return true;
  else
    return false;
#endif
#else
  std::string tmp = ws2s(path_file);
  struct stat st;
  if (0 == stat(tmp.c_str(), &st))
    if (st.st_mode & S_IFDIR)
      return true;
    else
      return false;
  else
    return false;
#endif
}

bool is_target_en_char(wchar_t wc)
{
  static std::wstring target_en_char(L" `~!@#$%^&*()-=+[]{};:'\"|,.<>?/");
  return std::wstring::npos != target_en_char.find(wc);
}

bool is_target_cn_char(wchar_t wc)
{
  // see below in visual studio "/？" need change to "？/"
  // or you will get error C2001: newline in constant
  static std::
    wstring
    target_cn_char
    (L" ·~！@#￥%……&*（）-——=+【】{}；：‘’“”、|，。《》？/");
  return std::wstring::npos != target_cn_char.find(wc);
}

void gen_new_name(std::wstring & file_name)
{
  std::replace_if(file_name.begin(), file_name.end(), is_target_en_char, '_');
  std::replace_if(file_name.begin(), file_name.end(), is_target_cn_char, '_');

  for (size_t i = 1; i < file_name.size(); i++) {
    if ('_' == file_name.at(i - 1) && '_' == file_name.at(i)) {
      file_name.erase(i, 1);
      i--;
    }
  }

  // delete underscore in the beginning of file name
  size_t pos = file_name.find_first_not_of('_');
  if (0 != pos)
    file_name.erase(0, pos);

  // delete underscore in the end of file name
  pos = file_name.find_last_of('_');
  if (pos == file_name.size() - 1)
    file_name.erase(pos, file_name.size() - pos);
}

void rename_file(const std::wstring & path_file)
{
  size_t pos = path_file.find_last_of('\\');
  std::wstring file_path = path_file.substr(0, pos + 1);
  std::wstring file_name = path_file.substr(pos + 1, path_file.size() - pos);
  pos = file_name.find_last_of('.');

  // ignore files that begin with a period, such as .gitignore .emacs
  if (0 == pos)
    return;

  std::wstring file_extn = file_name.substr(pos, file_name.size() - pos);
  file_name = file_name.substr(0, pos);

  gen_new_name(file_name);

  file_name.append(file_extn);
  std::wcout << "Org path file: " << path_file.c_str() << std::endl;
  file_path.append(file_name);
  std::wcout << "New path file: " << file_path.c_str() << std::endl;

  std::string tmp_old = ws2s(path_file);
  std::string tmp_new = ws2s(file_path);
  rename(tmp_old.c_str(), tmp_new.c_str());
}

void rename_dir(const std::wstring & path_file)
{
  std::wstring new_path(path_file);
  std::wcout << "Org path file: " << path_file.c_str() << std::endl;
  size_t pos = new_path.find_last_of('\\');
  if (pos == new_path.size() - 1)
    new_path.erase(pos, new_path.size() - pos);

  pos = new_path.find_last_of('\\');
  std::wstring dir_name = new_path.substr(pos + 1, new_path.size() - pos);
  new_path = new_path.substr(0, pos + 1);

  gen_new_name(dir_name);

  new_path.append(dir_name);
  std::wcout << "New path file: " << new_path.c_str() << std::endl;

  std::string tmp_old = ws2s(path_file);
  std::string tmp_new = ws2s(new_path);
  rename(tmp_old.c_str(), tmp_new.c_str());
}

#if defined(_MSC_VER)
#else
// FIXME: how to rename directory's name so that its sub-directories can be opened
// FIXME: the conversions between std::string and std::wstring are very poor
void linux_list_dirs_and_files(const std::wstring & path_file)
{
  DIR *dir;
  struct dirent *entry;
  std::string in_path_file = ws2s(path_file);

  if (!(dir = opendir(in_path_file.c_str()))) {
    std::wcout << "opendir " << in_path_file.c_str() << "error" << std::endl;
    return;
  }

  if (!(entry = readdir(dir))) {
    std::wcout << "readdir " << in_path_file.c_str() << "error" << std::endl;
    return;
  }

  do {
    if (DT_DIR == entry->d_type) {
      char path[1024] = { 0 };
      size_t len = snprintf(path, sizeof(path) - 1, "%s/%s",
			    in_path_file.c_str(), entry->d_name);
      path[len] = 0;
      if (0 == strcmp(entry->d_name, ".") || 0 == strcmp(entry->d_name, ".."))
        continue;
      std::string tmp(path);
      std::wstring w_tmp = s2ws(tmp);
      std::cout << "directory: " << entry->d_name << std::endl;
      linux_list_dirs_and_files(w_tmp);
    }
    else {
      std::cout << "file: " << entry->d_name << std::endl;
    }
  } while (entry = readdir(dir));

  closedir(dir);
}
#endif

void rename_dirs_and_files(const std::wstring & path_file)
{
  size_t files_count = 0, directories_count = 0;

#if defined(_MSC_VER)
#if _MSC_VER >= 1900
  // rename files
  for (std::tr2::sys::recursive_directory_iterator it(path_file.c_str()), end;
       it != end; ++it) {
    if (!std::tr2::sys::is_directory(it->path())) {
      rename_file(it->path());
      files_count++;
    }
  }

  // rename directories
  for (std::tr2::sys::recursive_directory_iterator it(path_file.c_str()), end;
       it != end; ++it) {
    if (std::tr2::sys::is_directory(it->path())) {
      rename_dir(it->path());
      directories_count++;
    }
  }

#else
  WIN32_FIND_DATAW wfd;
  ZeroMemory(&wfd, sizeof(wfd));

  std::wstring new_path(path_file);
  new_path.append(L"\\*.*");

  HANDLE h_file = FindFirstFileW(new_path.c_str(), &wfd);
  if (INVALID_HANDLE_VALUE == h_file) {
    std::cout << "FindFirstFileW error: " << GetLastError() << std::endl;
    return;
  }

  do {
    if ('.' == wfd.cFileName[0])
      continue;

    std::wstring str = path_file + L"\\" + wfd.cFileName;

    // rename files
    if (!file_is_directory(str)) {
      rename_file(str);
      files_count++;
    }
    // directories
    else {
      rename_dir(str);
      directories_count++;
    }
  } while (FindNextFileW(h_file, &wfd));

  FindClose(h_file);
#endif
#else
  // TODO: list all files and sub-directories in a directory in linux
  linux_list_dirs_and_files(path_file);
#endif

  // rename the last directory

  // need to comment out this code, if argv[1] is /aa/bb/cc, it will create
  // a new directory and named aa_bb_cc, at the same time, all the files in
  // cc will be moved to aa_bb_cc
  /*rename_dir(path_file); */

  std::cout << "1 directory, " << directories_count << " sub directories, ";
  std::cout << files_count << " files" << std::endl;
}
