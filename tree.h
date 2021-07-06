//Reads files with an indent-based hierarchical structure and transforms them into a tree

#pragma once


#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<deque>
#include<list>

class tree;


class tree_node {
public:
	std::string name;
	tree_node* parent = NULL;
	std::vector<std::string> data;
	std::vector<tree_node*> children;
	int parent_index = -1;
	std::vector<int> indices;

	std::string str(int depth = 0);
	std::string str(tree& t, int depth = 0);

	tree_node();
	tree_node(const tree_node& other);
	tree_node(const tree_node& other, tree& t);
};



class tree
{
public:
	static std::string join(const std::vector<std::string>& in, std::string separator);
	static void split(const std::string& in, std::vector<std::string>& out, std::string separator);
	static int first(const std::string& in, const std::string& value);
	static int first_not(const std::string& in, const std::string& chars);
	static std::string trim_start(const std::string& in, const std::string& chars);
	static std::string first_in_split(const std::string& in, const std::string& trim_chars, std::string separator);
	static int parse_int(const std::string& in, const int& def = 0);
	static float parse_float(const std::string& in, const float& def = 0.0f);
	template<class T>
	static T& element(const std::vector<T>& elements, std::string& index, const int& default_index = 0);
	template<class T>
	static T& element(const std::vector<T>& elements, int index, const int& default_index);
	template<class T>
	static T& empty_vector(const std::vector<T>& elements);


	tree();
	tree(std::string filepath, std::string header);
	tree(const tree& other);
	virtual ~tree();
	static tree load(std::string filepath, std::string header);
	std::string str(int depth = 0);

	std::string name;
	std::string data_name;
	std::string alt_name;
	std::string file_name;

	tree_node root;

	std::deque<tree_node> nodes;
	int node_index(tree_node* ptr) const;

	//void shrink();
	//void shrink_deep();


};



