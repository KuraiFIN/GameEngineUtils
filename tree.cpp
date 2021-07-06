//Reads files with an indent-based hierarchical structure and transforms them into a tree

#include "tree.h"


std::string tree::join(const std::vector<std::string>& in, std::string separator) {
	int len = int(in.size());
	if (len == 0) return "";
	std::string out = in[0];
	for (int i = 1; i != len; i++) out += separator + in[i];
	return out;
}
void tree::split(const std::string& in, std::vector<std::string>& out, std::string separator) {
	out.clear();
	std::string element;
	for (int i = 0; i != int(in.size()); i++) {
		if (i + separator.size() >= in.size()) {
			element += in.substr(i, separator.size());
			out.push_back(element);
			break;
		}
		std::string letter = in.substr(i, separator.size());
		if (letter == separator) {
			if (!element.empty()) {
				out.push_back(element);
				element.clear();
				i += (int)separator.size() - 1;
			}
			else out.push_back("");
		}
		else
			element += in[i];
	}
}
int tree::first(const std::string& in, const std::string& value) {
	for (int i = 0; i != int(in.size()); i++) {
		if (i + value.size() >= in.size()) {
			return -1;
		}
		std::string letter = in.substr(i, value.size());
		if (letter == value)
			return i;
	}
	return -1;
}
int tree::first_not(const std::string& in, const std::string& chars) {
	for (int i = 0; i != int(in.size()); i++) {
		const char letter = in[i];
		bool listed = false;
		for (int n = 0; n != int(chars.size()); n++) {
			const char c = chars[n];
			if (letter == c) {
				listed = true;
				break;
			}
		}
		if (!listed)
			return i;
	}
	return -1;
}
std::string tree::trim_start(const std::string& in, const std::string& chars) {
	int i = first_not(in, chars);
	if (i == -1)
		return in;
	return in.substr(i, in.size() - i);
}
std::string tree::first_in_split(const std::string& in, const std::string& trim_chars, std::string separator) {
	std::string trim = trim_start(in, trim_chars);
	int i = first(trim, " ");
	if (i == -1)
		return trim;
	return trim.substr(0, i);
}

int tree::parse_int(const std::string& in, const int& def) {
	try { return std::stoi(in); }
	catch (const std::exception& e) { return def; }
}
float tree::parse_float(const std::string& in, const float& def) {
	try { return std::stof(in); }
	catch (const std::exception& e) { return def; }
}
template<class T>
T& tree::element(const std::vector<T>& elements, std::string& index, const int& default_index) {
	int i = parse_int(index, 1) - 1;
	if (i >= elements.size()) i = default_index;
	return elements[i];
}
template<class T>
T& tree::element(const std::vector<T>& elements, int index, const int& default_index) {
	if (index >= elements.size()) return elements[default_index];
	return elements[index];
}

template <class T>
T& tree::empty_vector(const std::vector<T>& elements) {
	//.clear(); .erase();
	std::vector<T>().swap(elements);
	return elements;
}


tree::tree() {
	name = "?";
}
tree::tree(const tree& other) : name(other.name) {
	for (int i = 0; i != other.nodes.size(); i++)
		nodes.push_back(other.nodes[i]);
	root = tree_node(other.root, *this);
}
tree::~tree() {

}
tree::tree(std::string filepath, std::string header) {
	//tree output = tree();
	name = filepath;
	file_name = filepath;
	root = tree_node();
	root.name = filepath;

	std::ifstream file(filepath);
	if (!file.is_open()) return;

	tree_node* focus = &root;
	std::vector<tree_node*> overview;
	overview.reserve(10);//Not MANDATORY
	overview.emplace_back(focus);
	int depth = 0;

	bool indent_found = false;
	int indent = 2;
	std::string line;
	while (std::getline(file, line)) {
		if (line.size() == 0 || line[0] == '#' || line[0] == '/')
			continue;
		int d = line.size();
		line = trim_start(line, " \t");
		d = (d - line.size());
		if (!indent_found && d != 0) {
			indent = d;
			indent_found = true;
		}
		d /= indent;

		bool attach_same = true;
		bool attach_deeper = true;
		if (d > depth) attach_same = false;
		else if (d < depth) {
			attach_same = false;
			attach_deeper = false;
			while (overview.size() - 1 > d) {
				overview.erase(overview.begin() + overview.size() - 1);
				focus = overview[overview.size() - 1];
			}
		}

		std::vector<std::string> line_split;
		split(line, line_split, " ");
		if (line_split[0] != header) focus->data.emplace_back(line);
		else {
			depth = d;
			tree_node tn = tree_node();
			nodes.emplace_back(tn);
			tree_node* node = &nodes[nodes.size() - 1];
			node->name = line.substr(header.size() + 1);
			tree_node* parent = focus;

			if (nodes.size() > 1) node->parent_index = nodes.size() - 2;
			if ((attach_same || !attach_deeper) && focus->parent != NULL) {
				parent = focus->parent;
				node->parent_index = focus->parent_index;
			}

			node->parent = parent;
			parent->children.emplace_back(node);
			parent->indices.emplace_back(nodes.size() - 1);
			//std::cout << "Attaching child " + node->name + " to " + parent->name << "[" << parent->children.size() << "]" << std::endl;

			if (attach_same || !attach_deeper) overview[overview.size() - 1] = node;
			else overview.emplace_back(node);
			focus = node;
		}
	}
	file.close();
	//if (output.children.size() == 1) output = output.children[0];//SCREWS UP EVERYTHING!! Throws bad_alloc or lists immediate unnamed children with no grandchildren
	//output.shrink_deep();
	return;
}
tree tree::load(std::string filepath, std::string header) {
	return tree(filepath, header);
}


std::string tree_node::str(int depth) {
	std::string space = "";
	for (int i = 0; i != depth; i++)
		space += ".";
	std::string output = space + "o " + name + ":";
	for (int i = 0; i != data.size(); i++) {
		output += "\n" + space + data[i];
	}
	for (int i = 0; i != children.size(); i++) {
		std::string child = children[i]->str(depth + 1);
		output += "\n" + child;
	}
	return output;
}
std::string tree_node::str(tree& t, int depth) {
	std::string space = "";
	for (int i = 0; i != depth; i++)
		space += ".";
	std::string output = space + "o " + name + ":";
	for (int i = 0; i != data.size(); i++) {
		//output += "\n" + space + data[i];
	}
	for (int i = 0; i != indices.size(); i++) {
		std::string child = t.nodes[indices[i]].str(t, depth + 1);
		output += "\n" + child;
	}
	return output;
}
std::string tree::str(int depth) {
	return root.str(*this, depth);
}


int tree::node_index(tree_node* ptr) const {
	for (int i = 0; i != nodes.size(); i++) {
		if (&nodes[i] == ptr) {
			return i;
		}
	}
	return 0;
}


tree_node::tree_node() {
	name = "?";
	parent = NULL;
}
tree_node::tree_node(const tree_node& other) : name(other.name), parent(other.parent) {
	data.reserve(other.data.size());
	for (int i = 0; i != other.data.size(); i++)
		data.push_back(other.data[i]);
	children.reserve(other.children.size());
	for (int i = 0; i != other.children.size(); i++)
		children.emplace_back(other.children[i]);
	indices.reserve(other.indices.size());
	for (int i = 0; i != other.indices.size(); i++)
		indices.push_back(other.indices[i]);
}
tree_node::tree_node(const tree_node& other, tree& t) : name(other.name), parent(other.parent) {
	data.reserve(other.data.size());
	for (int i = 0; i != other.data.size(); i++)
		data.push_back(other.data[i]);
	indices.reserve(other.indices.size());
	children.reserve(other.indices.size());
	for (int i = 0; i != other.indices.size(); i++) {
		indices.push_back(other.indices[i]);
		children.push_back(&t.nodes[other.indices[i]]);
	}
}

