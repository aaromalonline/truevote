#include "mainwindow.h"
#include <QApplication>
#include <QGridLayout>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QInputDialog>
#include <QDateTime>
#include <QMap>
#include <QMessageBox>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

// SHA256 implementation
inline std::string sha256(const std::string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), pollOpen(false) {
    // Create data directory if it doesn't exist
    QDir().mkpath("./data");
    
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Create all pages
    stackedWidget->addWidget(createMainMenu());
    stackedWidget->addWidget(createAdminLogin());
    stackedWidget->addWidget(createVoterLogin());
    stackedWidget->addWidget(createAdminPanel());
    stackedWidget->addWidget(createVotingPage());

    showMainMenu();
    setWindowTitle("eVoting System");
    resize(400, 300);
}

QWidget* MainWindow::createMainMenu() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    QPushButton* adminButton = new QPushButton("Admin Login");
    QPushButton* voterButton = new QPushButton("Vote");
    QPushButton* exitButton = new QPushButton("Exit");

    connect(adminButton, &QPushButton::clicked, this, &MainWindow::showAdminLogin);
    connect(voterButton, &QPushButton::clicked, this, &MainWindow::showVoterLogin);
    connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    layout->addWidget(new QLabel("Welcome to eVoting System"));
    layout->addWidget(adminButton);
    layout->addWidget(voterButton);
    layout->addWidget(exitButton);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

QWidget* MainWindow::createAdminLogin() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    adminUserEdit = new QLineEdit;
    adminPassEdit = new QLineEdit;
    adminPassEdit->setEchoMode(QLineEdit::Password);

    QPushButton* loginButton = new QPushButton("Login");
    QPushButton* backButton = new QPushButton("Back");

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::handleAdminLogin);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showMainMenu);

    layout->addWidget(new QLabel("Username:"));
    layout->addWidget(adminUserEdit);
    layout->addWidget(new QLabel("Password:"));
    layout->addWidget(adminPassEdit);
    layout->addWidget(loginButton);
    layout->addWidget(backButton);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

QWidget* MainWindow::createVoterLogin() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    voterIdEdit = new QLineEdit;
    voterPassEdit = new QLineEdit;
    voterPassEdit->setEchoMode(QLineEdit::Password);

    QPushButton* loginButton = new QPushButton("Login");
    QPushButton* backButton = new QPushButton("Back");

    connect(loginButton, &QPushButton::clicked, this, &MainWindow::handleVoterLogin);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showMainMenu);

    layout->addWidget(new QLabel("Voter ID:"));
    layout->addWidget(voterIdEdit);
    layout->addWidget(new QLabel("Password:"));
    layout->addWidget(voterPassEdit);
    layout->addWidget(loginButton);
    layout->addWidget(backButton);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

QWidget* MainWindow::createAdminPanel() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    QPushButton* registerVoterBtn = new QPushButton("Register Voter");
    QPushButton* registerCandidateBtn = new QPushButton("Register Candidate");
    QPushButton* startPollBtn = new QPushButton("Start Poll");
    QPushButton* closePollBtn = new QPushButton("Close Poll");
    QPushButton* viewResultsBtn = new QPushButton("View Results");
    QPushButton* clearLogsBtn = new QPushButton("Clear Logs");
    QPushButton* logoutBtn = new QPushButton("Logout");

    connect(registerVoterBtn, &QPushButton::clicked, this, &MainWindow::registerVoter);
    connect(registerCandidateBtn, &QPushButton::clicked, this, &MainWindow::registerCandidate);
    connect(startPollBtn, &QPushButton::clicked, this, &MainWindow::startPoll);
    connect(closePollBtn, &QPushButton::clicked, this, &MainWindow::closePoll);
    connect(viewResultsBtn, &QPushButton::clicked, this, &MainWindow::viewResults);
    connect(clearLogsBtn, &QPushButton::clicked, this, &MainWindow::clearLogs);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::logout);

    layout->addWidget(registerVoterBtn);
    layout->addWidget(registerCandidateBtn);
    layout->addWidget(startPollBtn);
    layout->addWidget(closePollBtn);
    layout->addWidget(viewResultsBtn);
    layout->addWidget(clearLogsBtn);
    layout->addWidget(logoutBtn);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

QWidget* MainWindow::createVotingPage() {
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    candidateCombo = new QComboBox;
    QPushButton* voteButton = new QPushButton("Cast Vote");
    QPushButton* backButton = new QPushButton("Back");

    connect(voteButton, &QPushButton::clicked, this, &MainWindow::handleVote);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showMainMenu);

    layout->addWidget(new QLabel("Select Candidate:"));
    layout->addWidget(candidateCombo);
    layout->addWidget(voteButton);
    layout->addWidget(backButton);
    layout->addStretch();

    widget->setLayout(layout);
    return widget;
}

void MainWindow::showMainMenu() {
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showAdminLogin() {
    adminUserEdit->clear();
    adminPassEdit->clear();
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showVoterLogin() {
    if (!pollOpen) {
        QMessageBox::warning(this, "Error", "Poll is not open.");
        return;
    }
    voterIdEdit->clear();
    voterPassEdit->clear();
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::handleAdminLogin() {
    QString username = adminUserEdit->text();
    QString password = adminPassEdit->text();

    if (username == "admin" && password == "admin123") {
        stackedWidget->setCurrentIndex(3);  // Show admin panel
    } else {
        QMessageBox::warning(this, "Error", "Invalid credentials");
    }
}

void MainWindow::handleVoterLogin() {
    QString id = voterIdEdit->text();
    QString password = voterPassEdit->text();

    QFile votersFile("./data/voters.csv");
    if (!votersFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open voters file");
        return;
    }

    QTextStream in(&votersFile);
    bool found = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() < 3) continue;

        if (fields[0] == id && fields[2] == QString::fromStdString(sha256(password.toStdString()))) {
            // Check if already voted
            QFile logFile("./data/votes.log");
            if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream logStream(&logFile);
                bool hasVoted = false;
                
                while (!logStream.atEnd()) {
                    QString logLine = logStream.readLine();
                    if (logLine.startsWith(id + ",")) {
                        hasVoted = true;
                        break;
                    }
                }
                logFile.close();

                if (hasVoted) {
                    QMessageBox::warning(this, "Error", "You have already voted.");
                    votersFile.close();
                    return;
                }
            }

            found = true;
            break;
        }
    }
    votersFile.close();

    if (!found) {
        QMessageBox::warning(this, "Error", "Invalid credentials");
        return;
    }

    // Load candidates
    candidateCombo->clear();
    QFile candidatesFile("./data/candidates.csv");
    if (candidatesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream candStream(&candidatesFile);
        while (!candStream.atEnd()) {
            QString line = candStream.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 2) {
                candidateCombo->addItem(fields[1], fields[0]);
            }
        }
        candidatesFile.close();
    }

    stackedWidget->setCurrentIndex(4);  // Show voting page
}

void MainWindow::registerVoter() {
    bool ok;
    QString id = QInputDialog::getText(this, "Register Voter", "Enter Voter ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;

    QString name = QInputDialog::getText(this, "Register Voter", "Enter Voter Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString password = QInputDialog::getText(this, "Register Voter", "Set Password:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QFile file("./data/voters.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << id << "," << name << "," << QString::fromStdString(sha256(password.toStdString())) << "\n";
        file.close();
        QMessageBox::information(this, "Success", "Voter registered successfully");
    }
}

void MainWindow::registerCandidate() {
    bool ok;
    QString id = QInputDialog::getText(this, "Register Candidate", "Enter Candidate ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;

    QString name = QInputDialog::getText(this, "Register Candidate", "Enter Candidate Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QFile file("./data/candidates.csv");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << id << "," << name << "\n";
        file.close();
        QMessageBox::information(this, "Success", "Candidate registered successfully");
    }
}

void MainWindow::startPoll() {
    if (pollOpen) {
        QMessageBox::warning(this, "Error", "Poll is already open");
        return;
    }
    pollOpen = true;
    QMessageBox::information(this, "Success", "Poll started successfully");
}

void MainWindow::closePoll() {
    if (!pollOpen) {
        QMessageBox::warning(this, "Error", "Poll is not currently open");
        return;
    }
    pollOpen = false;
    QMessageBox::information(this, "Success", "Poll closed successfully");
}

void MainWindow::viewResults() {
    if (pollOpen) {
        QMessageBox::warning(this, "Error", "Cannot view results while poll is open");
        return;
    }

    QMap<QString, QString> candidates;
    QMap<QString, int> voteCount;

    // Load candidates
    QFile candidatesFile("./data/candidates.csv");
    if (candidatesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&candidatesFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 2) {
                candidates[fields[0]] = fields[1];
                voteCount[fields[0]] = 0;
            }
        }
        candidatesFile.close();
    }

    // Count votes
    QFile logFile("./data/votes.log");
    if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&logFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 2) {
                QString candidateId = fields[1];
                voteCount[candidateId]++;
            }
        }
        logFile.close();
    }

    // Display results
    QString results = "Election Results:\n\n";
    QMapIterator<QString, QString> i(candidates);
    while (i.hasNext()) {
        i.next();
        results += i.value() + ": " + QString::number(voteCount[i.key()]) + " votes\n";
    }

    // Save to file
    QFile resultFile("./data/result.txt");
    if (resultFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&resultFile);
        out << "Election Results\n================\n\n" << results;
        resultFile.close();
    }

    QMessageBox::information(this, "Results", results);
}

void MainWindow::clearLogs() {
    QFile logFile("./data/votes.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logFile.close();
    }

    QFile resultFile("./data/result.txt");
    if (resultFile.exists()) {
        resultFile.remove();
    }

    QMessageBox::information(this, "Success", "Logs cleared successfully");
}

void MainWindow::handleVote() {
    if (!pollOpen) {
        QMessageBox::warning(this, "Error", "Poll is not open");
        return;
    }

    QString candidateId = candidateCombo->currentData().toString();
    QString voterId = voterIdEdit->text();

    QFile file("./data/votes.log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << voterId << "," << candidateId << "," 
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        file.close();
    }

    QMessageBox::information(this, "Success", "Vote cast successfully");
    showMainMenu();
}

void MainWindow::logout() {
    showMainMenu();
}