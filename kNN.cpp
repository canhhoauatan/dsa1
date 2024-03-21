#include "kNN.hpp"

/* TODO: You can implement methods, functions that support your data structures here.
 * */
template <typename T>
class ListNode {
public:
    T val;
    ListNode* left;
    ListNode* right;
    ListNode(T x = 0, ListNode* l = nullptr, ListNode* r = nullptr) : val(x), left(l), right(r) {}
};

template <typename T>
class LinkedList : public List<T> {
private:
    ListNode<T>* head;
    int size;

public:
    LinkedList() : head(nullptr), size(0) {}

    ~LinkedList() {
        clear();
    }

    void push_back(T value) override {
        insert(size, value);
    }

    void push_front(T value) override {
        insert(0, value);
    }

    void insert(int index, T value) override {
        if (index < 0 || index > size) {
            return;
        }
        ListNode<T>* newNode = new ListNode<T>(value);
        if (index == 0) {
            newNode->right = head;
            if (head != nullptr) {
                head->left = newNode;
            }
            head = newNode;
        } else {
            ListNode<T>* current = head;
            for (int i = 0; i < index - 1; ++i) {
                current = current->right;
            }
            newNode->right = current->right;
            if (current->right != nullptr) {
                current->right->left = newNode;
            }
            current->right = newNode;
            newNode->left = current;
        }
        ++size;
    }

    void remove(int index) override {
        if (index < 0 || index >= size) {
            return;
        }
        ListNode<T>* temp;
        if (index == 0) {
            temp = head;
            head = head->right;
            if (head != nullptr) {
                head->left = nullptr;
            }
        } else {
            ListNode<T>* current = head;
            for (int i = 0; i < index - 1; ++i) {
                current = current->right;
            }
            temp = current->right;
            current->right = temp->right;
            if (temp->right != nullptr) {
                temp->right->left = current;
            }
        }
        delete temp;
        --size;
    }

    T& get(int index) const override {
        if (index < 0 || index >= size) {
            throw std::out_of_range("get(): Out of range");
        }
        ListNode<T>* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->right;
        }
        return current->val;
    }

    int length() const override {
        return size;
    }

    void clear() override {
        while (head != nullptr) {
            ListNode<T>* temp = head;
            head = head->right;
            delete temp;
        }
        size = 0;
    }

    void print() const override {
        ListNode<T>* current = head;
        while (current != nullptr) {
            std::cout << current->val << " ";
            current = current->right;
        }
        std::cout << std::endl;
    }

    void reverse() override {
        ListNode<T>* prev = nullptr;
        ListNode<T>* current = head;
        ListNode<T>* next = nullptr;
        while (current != nullptr) {
            next = current->right;
            current->right = prev;
            current->left = next;
            prev = current;
            current = next;
        }
        head = prev;
    }
};






Dataset::Dataset() : data(new LinkedList<List<int>*>()) {}

Dataset::~Dataset() {
    clear();
    delete data;
}

Dataset::Dataset(Dataset& other) : data(new LinkedList<List<int>*>(*(other.data))) {}

Dataset& Dataset::operator=(const Dataset& other) {
    if (this != &other) {
        clear();
        *data = *other.data;
    }
    return *this;
}

bool Dataset::loadFromCSV(const char* fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cell;
        List<int>* row = new LinkedList<int>();
        while (std::getline(iss, cell, ',')) {
            row->push_back(std::stoi(cell));
        }
        data->push_back(row);
    }

    file.close();
    return true;
}

void Dataset::printHead(int nRows, int nCols) const {
    int numRows = std::min(nRows, data->length());
    for (int i = 0; i < numRows; ++i) {
        List<int>* row = data->get(i);
        int numCols = std::min(nCols, row->length());
        for (int j = 0; j < numCols; ++j) {
            std::cout << row->get(j) << " ";
        }
        std::cout << std::endl;
    }
}

void Dataset::printTail(int nRows, int nCols) const {
    int numRows = data->length();
    for (int i = numRows - std::min(numRows, nRows); i < numRows; ++i) {
        List<int>* row = data->get(i);
        int numCols = row->length();
        for (int j = numCols - std::min(numCols, nCols); j < numCols; ++j) {
            std::cout << row->get(j) << " ";
        }
        std::cout << std::endl;
    }
}

void Dataset::getShape(int& nRows, int& nCols) const {
    nRows = data->length();
    if (nRows > 0) {
        nCols = data->get(0)->length();
    } else {
        nCols = 0;
    }
}

void Dataset::columns() const {
    if (data->length() > 0) {
        int numCols = data->get(0)->length();
        std::cout << "Number of columns: " << numCols << std::endl;
    } else {
        std::cout << "Dataset is empty." << std::endl;
    }
}

bool Dataset::drop(int axis, int index, string columns) {
    if (axis == 0) { // Drop row
        if (index >= 0 && index < data->length()) {
            delete data->get(index);
            data->remove(index);
            return true;
        } else {
            return false;
        }
    } else if (axis == 1) { // Drop column
        int colIndex = std::stoi(columns);
        int numRows = data->length();
        for (int i = 0; i < numRows; ++i) {
            List<int>* row = data->get(i);
            if (colIndex >= 0 && colIndex < row->length()) {
                row->remove(colIndex);
            } else {
                return false;
            }
        }
        return true;
    }
    return false;
}

Dataset Dataset::extract(int startRow, int endRow, int startCol, int endCol) const {
    Dataset extracted;
    int numRows = data->length();
    int numCols = (numRows > 0) ? data->get(0)->length() : 0;

    startRow = (startRow < 0) ? 0 : startRow;
    endRow = (endRow < 0 || endRow >= numRows) ? numRows - 1 : endRow;
    startCol = (startCol < 0) ? 0 : startCol;
    endCol = (endCol < 0 || endCol >= numCols) ? numCols - 1 : endCol;

    for (int i = startRow; i <= endRow; ++i) {
        List<int>* newRow = new LinkedList<int>();
        for (int j = startCol; j <= endCol; ++j) {
            newRow->push_back(data->get(i)->get(j));
        }
        extracted.data->push_back(newRow);
    }

    return extracted;
}

List<List<int>*>* Dataset::getData() const {
    return data;
}


    void Dataset::clear() {
        int numRows = data->length();
        for (int i = 0; i < numRows; ++i) {
            delete data->get(i);
        }
        data->clear();
    }




    void kNN::fit(const Dataset& X_train, const Dataset& y_train) {
        this->X_train = X_train;
        this->y_train = y_train;
    }

    Dataset kNN::predict(const Dataset& X_test) {
        Dataset y_pred;

        for (int i = 0; i < X_test.getData()->length(); ++i) {
            List<int>* row = new LinkedList<int>();
            for (int j = 0; j < X_test.getData()->get(i)->length(); ++j) {
                // Perform kNN algorithm to predict label for each row in X_test
                int label = predictLabel(X_test.getData()->get(i)->get(j));
                row->push_back(label);
            }
            y_pred.getData()->push_back(row);
        }

        return y_pred;
    }

    double kNN::score(const Dataset& y_test, const Dataset& y_pred) {
        if (y_test.getData()->length() != y_pred.getData()->length()) {
            throw std::invalid_argument("Size mismatch between y_test and y_pred");
        }

        int correctPredictions = 0;
        int totalPredictions = y_test.getData()->length();

        for (int i = 0; i < totalPredictions; ++i) {
            List<int>* rowTest = y_test.getData()->get(i);
            List<int>* rowPred = y_pred.getData()->get(i);
            for (int j = 0; j < rowTest->length(); ++j) {
                if (rowTest->get(j) == rowPred->get(j)) {
                    correctPredictions++;
                }
            }
        }

        return static_cast<double>(correctPredictions) / totalPredictions;
    }


    int kNN::predictLabel(const List<int>* testData) {

        return 0;
    }


void train_test_split(Dataset& X, Dataset& y, double test_size, 
                        Dataset& X_train, Dataset& X_test, Dataset& y_train, Dataset& y_test) {
    if (X.getData()->length() != y.getData()->length()) {
        throw std::invalid_argument("Size mismatch between X and y");
    }

    int numSamples = X.getData()->length();
    int numTestSamples = static_cast<int>(numSamples * test_size);
    int numTrainSamples = numSamples - numTestSamples;

    List<List<int>*>* X_data = X.getData();
    List<List<int>*>* y_data = y.getData();

    List<List<int>*>* X_train_data = new LinkedList<List<int>*>();
    List<List<int>*>* X_test_data = new LinkedList<List<int>*>();
    List<List<int>*>* y_train_data = new LinkedList<List<int>*>();
    List<List<int>*>* y_test_data = new LinkedList<List<int>*>();    

    vector<int> testIndices;
    for (int i = 0; i < numTestSamples; ++i) {
        int index = rand() % numSamples;
        while (std::find(testIndices.begin(), testIndices.end(), index) != testIndices.end()) {
            index = rand() % numSamples;
        }
        testIndices.push_back(index);
    }

    for (int i = 0; i < numSamples; ++i) {
        if (std::find(testIndices.begin(), testIndices.end(), i) != testIndices.end()) {
            X_test_data->push_back(X_data->get(i));
            y_test_data->push_back(y_data->get(i));
        } else {
            X_train_data->push_back(X_data->get(i));
            y_train_data->push_back(y_data->get(i));
        }
    }

    X_train = Dataset(X_train_data);
    X_test = Dataset(X_test_data);
    y_train = Dataset(y_train_data);
    y_test = Dataset(y_test_data);
}

