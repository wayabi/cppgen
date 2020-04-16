#ifndef __U_CLASS_INFO__
#define __U_CLASS_INFO__

#include <string>
#include <vector>
#include <memory>

class func_info {
public:
	std::string name_func_;
	std::string return_func_;
};

class class_info {
public:
	std::string name_class_;
	std::vector<std::shared_ptr<func_info>> func_;
};

class header_info {
public:
	std::string dir_header_;
	std::string name_header_;
	std::vector<std::shared_ptr<class_info>> class_;

public:
	void parse(const char* path);
	bool make_cpp(bool force_overwrite);

	static std::string get_class(const std::string& s);
	static std::shared_ptr<func_info> get_func(const std::string& s, std::shared_ptr<class_info> class_info_current);
	static bool is_end_of_class(const std::string& s);

};

#endif
