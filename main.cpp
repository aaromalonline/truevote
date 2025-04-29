// g++ main.cpp -lcrypto

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <openssl/sha.h>

using namespace std;

string sha256(const string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

class Voter {
public:
    string id;
    string name;
    string passwordHash;
    bool hasVoted;

    Voter(string _id = "", string _name = "", string _pass = "") : id(_id), name(_name), passwordHash(sha256(_pass)), hasVoted(false) {}
};

class Candidate {
public:
    string id;
    string name;
    int votes;

    Candidate(string _id = "", string _name = "") : id(_id), name(_name), votes(0) {}
};

class PollSystem {
private:
    unordered_map<string, Voter> voters;
    unordered_map<string, Candidate> candidates;
    bool pollOpen = false;
    const string adminUser = "admin";
    const string adminPass = "admin123";

    bool validateAdmin(const string &user, const string &pass) {
        return user == adminUser && pass == adminPass;
    }

    void saveVotersToFile() {
        ofstream out("voters.csv");
        for (auto &v : voters) {
            out << v.second.id << "," << v.second.name << "," << v.second.passwordHash << "," << v.second.hasVoted << "\n";
        }
    }

    void saveCandidatesToFile() {
        ofstream out("candidates.csv");
        for (auto &c : candidates) {
            out << c.second.id << "," << c.second.name << "," << c.second.votes << "\n";
        }
    }

    void logVote(const string &voterId, const string &candidateId) {
        ofstream log("votes.log", ios::app);
        time_t now = time(0);
        log << voterId << "," << candidateId << "," << put_time(localtime(&now), "%F %T") << "\n";
    }

    void loadVoters() {
        ifstream in("voters.csv");
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string id, name, passHash, votedStr;
            getline(ss, id, ','); getline(ss, name, ','); getline(ss, passHash, ','); getline(ss, votedStr);
            Voter v(id, name); v.passwordHash = passHash; v.hasVoted = votedStr == "1";
            voters[id] = v;
        }
    }

    void loadCandidates() {
        ifstream in("candidates.csv");
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string id, name, votesStr;
            getline(ss, id, ','); getline(ss, name, ','); getline(ss, votesStr);
            candidates[id] = Candidate(id, name);
            candidates[id].votes = stoi(votesStr);
        }
    }

public:
    PollSystem() {
        loadVoters();
        loadCandidates();
    }

    void adminPanel() {
        string user, pass;
        cout << "\nAdmin Login\nUsername: ";
        cin >> user;
        cout << "Password: ";
        cin >> pass;

        if (!validateAdmin(user, pass)) {
            cout << "\nInvalid credentials.\n";
            return;
        }

        int choice;
        do {
            cout << "\nAdmin Panel:\n";
            cout << "1. Register Voter\n2. Register Candidate\n3. Start Poll\n4. Close Poll\n5. View Results\n0. Logout\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: registerVoter(); break;
                case 2: registerCandidate(); break;
                case 3: startPoll(); break;
                case 4: closePoll(); break;
                case 5: viewResults(); break;
                case 0: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    void registerVoter() {
        string id, name, password;
        cout << "Enter Voter ID: "; cin >> id;
        cout << "Enter Voter Name: "; cin >> name;
        cout << "Set Voter Password: "; cin >> password;
        voters[id] = Voter(id, name, password);
        saveVotersToFile();
        cout << "Voter registered.\n";
    }

    void registerCandidate() {
        string id, name;
        cout << "Enter Candidate ID: "; cin >> id;
        cout << "Enter Candidate Name: "; cin >> name;
        candidates[id] = Candidate(id, name);
        saveCandidatesToFile();
        cout << "Candidate registered.\n";
    }

    void startPoll() {
        if (pollOpen) {
            cout << "Poll is already open.\n";
        } else {
            pollOpen = true;
            cout << "Poll started. Voters can now vote.\n";
        }
    }

    void closePoll() {
        if (!pollOpen) {
            cout << "Poll is not currently open.\n";
        } else {
            pollOpen = false;
            cout << "Poll closed. Voting ended.\n";
        }
    }

    void vote() {
        if (!pollOpen) {
            cout << "Poll is not open.\n";
            return;
        }

        string id, pass;
        bool validCredentials = false;

        while (!validCredentials) {
            cout << "Enter Voter ID (or 'q' to quit): "; 
            cin >> id;
            
            if (id == "q") {
                cout << "Voting cancelled.\n";
                return;
            }

            cout << "Enter Password: "; 
            cin >> pass;

            if (voters.find(id) == voters.end() || voters[id].passwordHash != sha256(pass)) {
                cout << "Invalid voter ID or password. Please try again.\n";
                continue;
            }

            if (voters[id].hasVoted) {
                cout << "You have already voted.\n";
                return;
            }

            validCredentials = true;
        }

        cout << "Candidates:\n";
        for (auto &c : candidates) {
            cout << c.first << " - " << c.second.name << "\n";
        }

        string cid;
        cout << "Enter Candidate ID to vote: "; cin >> cid;

        if (candidates.find(cid) == candidates.end()) {
            cout << "Candidate not found.\n";
            return;
        }

        candidates[cid].votes++;
        voters[id].hasVoted = true;
        saveVotersToFile();
        saveCandidatesToFile();
        logVote(id, cid);
        cout << "Vote cast successfully.\n";
    }

    void viewResults() {
        if (pollOpen) {
            cout << "Poll is still open. Close it to view results.\n";
            return;
        }

        cout << "Results:\n";
        for (auto &c : candidates) {
            cout << c.second.name << ": " << c.second.votes << " votes\n";
        }
    }
};

int main() {
    PollSystem system;
    int choice;

    do {
        cout << "\nWelcome to eVoting System\n";
        cout << "1. Admin Panel\n2. Vote\n0. Exit\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.adminPanel(); break;
            case 2: system.vote(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}
