#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <sstream>
#include <bitset>
using namespace std;
class Node
{
	/* Private Data Member */
	char ch;
	int freq;
	Node *LNode, *RNode;

public:
	Node() {} // Default Constructor
	Node(char ch, int freq, Node *LNode, Node *RNode)
	{
		this->ch = ch;
		this->freq = freq;
		this->LNode = LNode;
		this->RNode = RNode;
	}
	/* Giving access to the functions */
	friend class comp;
	friend Node *Design_huffman_tree(string);
	string Dcode(string, Node *);
	void Ncode(Node *, string, unordered_map<char, string> &Hcode);
};
class comp
{
public:
	/* Comparator is used to sort the Priority Queue */
	bool operator()(Node *l, Node *r)
	{
		return l->freq > r->freq;
	}
};
Node *Design_huffman_tree(string text)
{
	/*
	unordered_map<char,int> to store the frequency of each character present in the string text
	*/
	unordered_map<char, int> freq;
	for (char ch : text)
	{
		freq[ch]++;
	}
	priority_queue<Node *, vector<Node *>, comp> pq;
	/*

	Minimum Prioirty_Queue to maintain
			-->> most frequently used letter have the shortest encoding and vice-versa <<--
	*/
	for (auto pair : freq)
	{
		Node *ptr = new Node(pair.first, pair.second, NULL, NULL);
		pq.push(ptr);
	}

	while (pq.size() != 1)
	{
		Node *left = pq.top();
		pq.pop();
		Node *right = pq.top();
		pq.pop();
		pq.push(new Node('\0', left->freq + right->freq, left, right));
	}
	return pq.top() /* root Node */;
}
void Node ::Ncode(Node *root, string str, unordered_map<char, string> &Decoded_Map)
{
	if (!root)
		return;
	/*
		storing the paths from root to leaf of each characters, and mapping into a STL unordered_map
		The Idea Used is Simple:
			if we are traversing to left from current Node we will append 0, else we will append 1
	*/
	if (!root->LNode && !root->RNode)
	{
		/*
		Mapping the value in unordered_map for leaf->ch
		*/
		Decoded_Map[root->ch] = str;
		return;
	}
	str.push_back('1');
	Ncode(root->LNode, str, Decoded_Map);
	str.pop_back();
	str.push_back('0');
	Ncode(root->RNode, str, Decoded_Map);
	str.pop_back();
}
string Node ::Dcode(string to_decode, Node *root)
{
	/*
	   Decoding : we have to pointers (IthIndex, temp) and a pointer to the root varialbe.
	   while, Traversing the to_decode binary string if to_decode[IthIndex] == 0, we will move the pointer temp
	   its left child's Node. Once we reech the leaf node we will store the character present in the Node and
	   move the temp pointer to root, and so on..................
										(root)
									  0 /    \ 1
									   /      \
								   (LNode)  (RNode)
	*/
	Node *temp = root;
	int IthIndex = 0;
	string decoded_str = "";
	int d_size = to_decode.size();
	while (IthIndex <= d_size)
	{
		if (!temp->RNode && !temp->LNode)
		{
			decoded_str = decoded_str + temp->ch; /* if root then we got one character from the data */
			temp = root;
		}
		else if (to_decode[IthIndex] == '1')
		{
			temp = temp->LNode;
			IthIndex++;
		}
		else
		{
			temp = temp->RNode;
			IthIndex++;
		}
	}
	return decoded_str;
}
int main()
{
	Node HuffmanNode; /* Initiliazating of Huffman code */

	cout << "Wait : Importing Data !!! " << endl;

	/* Importing .txt file and storing into inputfile */
	ifstream inputfile("Original_text_File.txt");
	if (!inputfile.is_open())
	{
		cout << "Error in Opening the input file." << endl;
		return 1;
	}

	/* Storing the imported .txt file into string Original_Text */
	string Original_Text;
	if (inputfile)
	{
		ostringstream ss;
		ss << inputfile.rdbuf();
		Original_Text = ss.str();
	}
	inputfile.close();

	/* Building of Huffman Tree */
	Node *root = Design_huffman_tree(Original_Text);

	/*......................***....................***ENCODING***.......................***..................*/

	/* Encoding the each characters present in the string to s sequence of unique Binary Numbers */
	string str = "";
	unordered_map<char, string> Decoded_Map;
	cout << "Wait : Encoding !!! " << endl;
	HuffmanNode.Ncode(root, str, Decoded_Map);

	/* Storing the String in terms of Binary digits with the help of Encoded data to a newly created EncodedFile.txt file */
	string compressedText = "";
	for (auto x : Original_Text)
	{
		compressedText += Decoded_Map[x];
	}

	/*
	Initialization of a Binary file
	*/
	string CompressedData = "compressed_Text.bin";
	ofstream compressedFile(CompressedData, ios::out | ios ::binary);
	if (!compressedFile.is_open())
	{
		/* If file is unable to get created */
		cout << "Error in Creating the output file." << endl;
		return 1;
	}

	/* Storing the binary string in .bin file with 8 bits */
	for (size_t i = 0; i < compressedText.length(); i += 8)
	{
		string byteStr = compressedText.substr(i, 8);
		compressedFile << static_cast<char>(bitset<8>(byteStr).to_ulong());
	}
	compressedFile.close();

	/*

	The Above compressedFile.txt file is a Binary file represents Compressed File form of Original data

	*/

	/*......................***....................***DECODING***.......................***..................*/

	/* Importing the Compressed & Encoded data to Decode */
	ifstream Compressed_file("compressed_Text.bin", ios ::binary);
	string to_decode;
	if (!Compressed_file.is_open())
	{
		/* If file is unable to get created */
		cout << "Error in Opening the Compressed file." << endl;
		return 1;
	}

	cout << "Wait : Importing Compressed Binary File !!! " << endl;
	/* Extraction of data from .bin file to string to_decode */
	char byte;
	while (Compressed_file.read(&byte, 1))
	{
		to_decode += bitset<8>(byte).to_string();
	}

	cout << "Wait : Decoding !!!" << endl;

	/* Calling the Decode function with input as Binary string and root of Huffman Tree */
	string decoded_Text = HuffmanNode.Dcode(to_decode, root);

	/* Storing yhe Decoded data into MyDecodedFile */
	ofstream MyDecodedFile("Decoded_File.txt");
	for (auto x : decoded_Text)
	{
		MyDecodedFile << x;
	}
	MyDecodedFile.close();
	int exit;
	cout << "Press Any Key to Exit : ";
	cin >> exit;
	return 0;
}