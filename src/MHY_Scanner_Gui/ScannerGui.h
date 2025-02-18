﻿#pragma once

#include <vector>

#include <QtWidgets/QMainWindow>
#include <QRunnable>
#include <Json.h>

#include "ui_ScannerGui.h"
#include "QRCodeForScreen.h"
#include "QRCodeForStream.h"
#include "LiveStreamLink.h"
#include "Common.h"
#include "ConfigDate.h"
#include "About_QDialog.h"

class OnlineUpdate :public QThread
{
public:
	OnlineUpdate() = default;
	void run();
	~OnlineUpdate();
};

class configInitLoad :public QThread
{
	Q_OBJECT
public:
	configInitLoad() = default;
	void run();
	~configInitLoad();
signals:
	void userinfoTrue(bool b);
private:
	ConfigDate* m_config = &ConfigDate::getInstance();
};

class ScannerGui
	: public QMainWindow
{
	Q_OBJECT

public:
	ScannerGui(QWidget* parent = nullptr);
	~ScannerGui();
	void insertTableItems(QString uid, QString userName, QString type, QString notes);
	virtual void closeEvent(QCloseEvent* event);
	virtual void showEvent(QShowEvent* event);
public slots:
	void LoginAccount();
	void SetDefaultAccount();
	void DeleteAccount();
	void About();
	void help();

	void pBtstartScreen();
	void pBtStream();
	void pBtStop();

	void checkBoxAutoScreen(bool clicked);
	void checkBoxAutoExit(bool clicked);
	void checkBoxAutoLogin(bool clicked);

	void islogin(const ScanRet::Type ret);
	void loginConfirmTip(const GameType::Type gameType, bool b);
	void getInfo(int x, int y);
	void configInitUpdate(bool b);
	void updateNote(QTableWidgetItem* item);
private:
	void failure();
	int countA = -1;
	Ui::ScannerGuiClass ui;
	ConfigDate* m_config = &ConfigDate::getInstance();
	json::Json userinfo;
	QRCodeForScreen t1;
	ThreadStreamProcess t2;
	int liveIdError(const LiveStreamStatus::Status data);
	int getSelectedRowIndex();
	bool checkDuplicates(const std::string uid);
	bool getStreamLink(const std::string& roomid, std::string& url, std::map<std::string, std::string>& heards);
	OnlineUpdate o;
	configInitLoad configinitload;
};