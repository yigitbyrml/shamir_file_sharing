#include <bits/stdc++.h>
using namespace std;

int evaluate_polynomial(const vector<int>& coeffs, int x) {
    int result = 0;
    int power = 1;
    for (int coeff : coeffs) {
        result += coeff * power;
        power *= x;
    }
    return result;
}

int interpolate(const vector<pair<int, int>>& shares) {
    int result = 0;
    int size = shares.size();
    for (int i = 0; i < size; i++) {
        int xi = shares[i].first;
        int yi = shares[i].second;
        int term = yi;

        for (int j = 0; j < size; j++) {
            if (i != j) {
                int xj = shares[j].first;
                term *= -xj;
                term /= (xi - xj);
            }
        }
        result += term;
    }
    return result;
}

vector<string> split_file(const string& filename, int n, int k) {
    ifstream file(filename, ios::binary);
    if (!file) {
        throw runtime_error("File could not be opened!");
    }

    vector<char> file_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    int secret = file_content.size();
    
    vector<int> coefficients(k);
    coefficients[0] = secret;
    for (int j = 1; j < k; ++j) {
		int p = 0;
		while (p == 0)
			p = (rand() % 997);

		coefficients[j] = p;
	}

    vector<string> share_files;
    for (int i = 1; i <= n; i++) {
        int y = evaluate_polynomial(coefficients, i);
        string share_file = filename + "_share_" + to_string(i);
        ofstream share(share_file, ios::binary);
        share << i << " " << y;
        share_files.push_back(share_file);
        share.close();
    }

    cout << "File split into " << n << "parts.\n";
    return share_files;
}


void reconstruct_file(const vector<string>& share_files, int k, const string& output_file) {
    vector<pair<int, int>> shares;

    for (const auto& share_file : share_files) {
        ifstream share(share_file, ios::binary);
        if (!share) {
            throw runtime_error("File could not be opened: " + share_file);
        }
        int x, y;
        share >> x >> y;
        shares.emplace_back(x, y);
        if (shares.size() == k) {
            break;
        }
    }

    if (shares.size() < k) {
        throw runtime_error("Not enough shares found!");
    }

    int secret = interpolate(shares);
    cout << "Secret (file size): " << secret << " Converted again as byte.\n";

    ofstream output(output_file, ios::binary);
    output<<secret;
    output.close();

    cout << "File successfully recreated: " << output_file << "\n";
}

int main() {
    try {
        cout << "1. Shred file\n2. Recreate file\nMake your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            string filename;
            int n, k;

            cout << "Enter the file name to be shredded: ";
            cin >> filename;
            cout << "How many parts should it be divided into (n): ";
            cin >> n;
            cout << "Threshold value (k): ";
            cin >> k;

            auto shares = split_file(filename, n, k);
            cout << "Share files:\n";
            for (const auto& share : shares) {
                cout << share << "\n";
            }
        } else if (choice == 2) {
            int k;
            vector<string> share_files;
            string share_file;
            string output_file;

            cout << "How many shares will be used (k): ";
            cin >> k;

            cout << "Enter the share files (type 'done' to finish):\n";
            while (true) {
                cin >> share_file;
                if (share_file == "done") break;
                share_files.push_back(share_file);
            }

            cout << "File name to be recreated: ";
            cin >> output_file;

            reconstruct_file(share_files, k, output_file);
        } else {
            cout << "Invalid selection!\n";
        }
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
