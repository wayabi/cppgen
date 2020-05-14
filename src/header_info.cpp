#include "header_info.h"
#include <stdio.h>
#include <utility>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include "Util.h"

using namespace std;
using namespace boost;

std::string header_info::get_class(const std::string& s)
{
	auto ss = Util::split(s, ' ');
	bool next_is_class = false;
	for(auto ite = ss.begin();ite != ss.end();++ite){
		if(next_is_class){
			return *ite;
		}
		if(*ite == "class"){
			next_is_class = true;
		}
	}
	return string("");
}

std::shared_ptr<func_info> header_info::get_func(const std::string& s, std::shared_ptr<class_info> class_info_current){
	//\\S not space char
	regex r("\\S+\\([^)]*\\)");
	smatch m;
	if(regex_search(s, m, r)){
		int pos = m.position();
		string name = m.str();
		int pos_end = pos-2;
		for(int i=pos_end;i>=-1;--i){
			if(i==-1 || *(s.c_str()+i) == '\t'){
				const char* c = s.c_str();
				string returns = string(c+i+1, c+pos_end+1);
				auto fi = make_shared<func_info>();
				fi->name_func_ = name;
				fi->return_func_ = returns;
				return fi;
			}
		}
	}
	if(class_info_current != nullptr){
		string class_name = class_info_current->name_class_;
		regex r2(string("~?")+class_name + string("\\([^)]*\\)"));
		smatch m;
		if(regex_search(s, m, r2)){
			int pos = m.position();
			auto fi = make_shared<func_info>();
			fi->name_func_ = m.str();
			fi->return_func_ = "";
			return fi;
		}
	}
	return nullptr;
}

bool header_info::is_end_of_class(const std::string& s)
{
	return (*(s.c_str()+0) == '}');
}

void header_info::parse(const char* path){
	FILE* f = fopen(path, "r");
	vector<char> buf;
	int size_buf = 1024*16;
	buf.resize(size_buf);

	if(f == NULL){
		cerr << "no such file: " << path << endl;
		return;
	}

	string s_path(path);
	auto ss_path = Util::split(s_path, '/');
	string name_part = ss_path[ss_path.size()-1];
	string dir_part = "./";
	if(name_part != s_path){
		dir_part = string(s_path.c_str(), s_path.c_str()+s_path.size()-name_part.size());
	}
	auto ss_name_part = Util::split(name_part, '.');
	if(ss_name_part.size() > 1){
		name_part = ss_name_part[0];
	}

	dir_header_ = dir_part;
	name_header_ = name_part;

	std::shared_ptr<class_info> class_info_current = nullptr;
	
	char* ccc;
	std::shared_ptr<func_info> func_info0 = nullptr;
	while((ccc	= fgets(&buf[0], size_buf, f)) != NULL){
		string s = Util::trim(string(&buf[0]));
		cout << s << endl;
		if(class_info_current == nullptr){
			string name = get_class(s);
			if(name.size() > 0){
				cout << "class found:" << name << endl;
				class_info_current = make_shared<class_info>();
				class_info_current->name_class_ = name;
				class_.push_back(class_info_current);
			}
		}else if((func_info0 = get_func(s, class_info_current)) != nullptr){
			class_info_current->func_.push_back(func_info0);
			cout << "func found:" << func_info0->name_func_ << endl;
		}else if(is_end_of_class(s)){
			class_info_current = nullptr;
			cout << "eoc" << endl;
		}
	}

	fclose(f);
}

bool header_info::make_cpp(bool force_overwrite)
{
	if(name_header_.size() == 0){
		cerr << "invalid name_header:" << name_header_ << endl;
		return false;
	}
	stringstream ss_path_out;
	ss_path_out << dir_header_ << "/" << name_header_ << ".cpp";
	string s_path_out = ss_path_out.str();

	if(!force_overwrite && Util::existFile(s_path_out.c_str())){
		cerr << "already exist: " << s_path_out << endl;
		return false;
	}

	FILE* f = fopen(s_path_out.c_str(), "w");
	if(f == NULL){
		cerr << "file open error:" << s_path_out << endl;
		return false;
	}

	stringstream ss;
	ss	<< std::uppercase << name_header_;
	string name_upper = ss.str();
	
	fprintf(f, "#include \"%s.h\"\n", name_header_.c_str());
	fprintf(f, "#include <iostream>\n");
	fprintf(f, "#include <string>\n");
	fprintf(f, "\n");
	fprintf(f, "using namespace std;\n");
	fprintf(f, "\n");
	for(auto ite = class_.begin();ite != class_.end();++ite){
		for(auto ite2 = (*ite)->func_.begin();ite2 != (*ite)->func_.end();++ite2){
			if((*ite2)->return_func_.size() > 0){
				fprintf(f, "%s %s::%s\n{\n", (*ite2)->return_func_.c_str(), (*ite)->name_class_.c_str(), (*ite2)->name_func_.c_str());
			}else{
				fprintf(f, "%s::%s\n{\n", (*ite)->name_class_.c_str(), (*ite2)->name_func_.c_str());
			}
			fprintf(f, "\n");
			fprintf(f, "}\n");
			fprintf(f, "\n");
		}
	}

	fclose(f);
	return true;
}
