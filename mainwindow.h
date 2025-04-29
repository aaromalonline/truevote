#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QComboBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showAdminLogin();
    void showVoterLogin();
    void handleAdminLogin();
    void handleVoterLogin();
    void showMainMenu();
    void registerVoter();
    void registerCandidate();
    void startPoll();
    void closePoll();
    void viewResults();
    void clearLogs();
    void handleVote();
    void logout();

private:
    QStackedWidget *stackedWidget;
    QWidget *createMainMenu();
    QWidget *createAdminLogin();
    QWidget *createVoterLogin();
    QWidget *createAdminPanel();
    QWidget *createVotingPage();
    
    QLineEdit *adminUserEdit;
    QLineEdit *adminPassEdit;
    QLineEdit *voterIdEdit;
    QLineEdit *voterPassEdit;
    QComboBox *candidateCombo;
    bool pollOpen;
};

#endif