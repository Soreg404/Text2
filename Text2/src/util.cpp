#include "util.h"

std::vector<char> ldFile(const wchar_t *path) {

	using std::ios;

	std::fstream file(path, ios::in | ios::binary | ios::ate);
	if(!file.good()) { LOG("couldn't open file");  return { 0 }; }
	size_t fsize = file.tellg();
	file.clear();
	file.seekg(file.beg);

	std::vector<char> ret(fsize + 1);

	if(fsize > 50000) { LOG("file too large");  return { 0 }; }
	file.read(ret.data(), fsize);
	ret[fsize] = 0;

	return ret;

}