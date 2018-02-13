#include "mainwindow.h"
#include <QRegExp>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	resize(400, 300);
	setMinimumWidth(400);
	setMinimumHeight(300);
	setMaximumWidth(400);
	setMaximumHeight(300);
	setWindowTitle("Client");

	/*pSocket = new QTcpSocket(this);
	connect(pSocket, SIGNAL(readyRead()), this, SLOT(sockReady()));
	connect(pSocket, SIGNAL(disconnected()), this, SLOT(sockDisc()));*/

	switchToLoginIn();

	// connect(this, SIGNAL(sendQuitSignal()), this, SLOT(recieveQuit()));
}

MainWindow::~MainWindow()
{
	sockDisc();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (pLobby->gameType != WrongGameType)
	{
		if (pLobby->host->playerName == pPlayer->playerName)
		{
			deleteLobbySlot();
		}
		else
		{
			leaveLobby();
		}
	}
	event->accept();
}

void MainWindow::connectToTheServer()
{
	if (!pSocket || pSocket->state() != QAbstractSocket::ConnectedState)
	{
		pSocket = new QTcpSocket(this);
		connect(pSocket, SIGNAL(readyRead()), this, SLOT(sockReady()));
		connect(pSocket, SIGNAL(disconnected()), this, SLOT(sockDisc()));
		connect(pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(sockDisc()));
		pSocket->connectToHost(passwordEdit->text(), 5555);
	}
	else
		QMessageBox::information(this, tr("Error"), "You are already connected, please Log in");
	// pSocket->deleteLater();
}

void MainWindow::sendLogin()
{
	if (pSocket->isOpen() && pSocket->state() == QAbstractSocket::ConnectedState)
	{
		QByteArray arrBlock;
		QDataStream out(&arrBlock, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_5_9);

		CommandType commandType = {CommandType::Type::AskLogin};
		Command* pCommand = new Login(loginEdit->text());

		// pPlayer = new Player(loginEdit->text());
		// qDebug() << int(pSocket->socketDescriptor());
		// player->setID(int(pSocket->socketDescriptor()));

		out << commandType;
		pCommand->operator<<(out);
		pSocket->write(arrBlock);
		pSocket->waitForBytesWritten();

		qDebug() << "Login Command Sent";
	}
	else
		QMessageBox::information(this, "Error", "You must connect first");
}

void MainWindow::createLobbyDialog()
{
	CreateLobbyDialog* dialog = new CreateLobbyDialog(pPlayer->playerName);
	connect(dialog, SIGNAL(clicked(QString, QString, int)), this, SLOT(sendCreateLobby(QString, QString, int)));
	connect(this, SIGNAL(closeGame()), dialog, SLOT(closeSlot()));
}

void MainWindow::sendConnectToLobbySlot(QTreeWidgetItem* item, int column)
{
	sendConnectToLobby(new Lobby(item->text(0)), pPlayer, true);
}

void MainWindow::leaveLobby()
{
	// sendUpdateLobby(pLobby->gameType, Unready);
	sendConnectToLobby(pLobby, pPlayer, false);
	pLobby = new Lobby();
	switchToMain();
}

void MainWindow::deleteLobbySlot()
{
	if (pLobby->gameType != WrongGameType)
		deleteLobby(pLobby);
	// switchToMain();
}

void MainWindow::leaveLobbiesList()
{
	switchToMain();
}

void MainWindow::setRdy()
{
	// qDebug() << connectedPlayersList->item(1)->text();
	if (connectedPlayersList->item(1)->checkState())
	{
		connectedPlayersList->item(1)->setCheckState(Qt::Unchecked);
		pLobby->updateStatus(Unready);
	}
	else
	{
		connectedPlayersList->item(1)->setCheckState(Qt::Checked);
		pLobby->updateStatus(Ready);
	}
}

void MainWindow::sendRdy()
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::SendRdy};
	Command* pCommand = new SendRdy(pLobby->host);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "SendRdy Command Sent";
}

void MainWindow::sendMessage(QString message, bool errorFlag, QString playerName)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::SendMessage};
	Command* pCommand = new SendMessage(message, errorFlag, playerName);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "SendMessage Command Sent";
}

void MainWindow::switchToCoridorWindow(bool hosting)
{
	this->hide(); // Закрываем основное окно
	if (hosting)
	{
		if (!pLobby->connectedPlayers.empty())
		{
			dialogChoosePlayer = new DialogChoosePlayer(hosting, pLobby->host->playerName, pLobby->connectedPlayers.back()->playerName);
			connect(dialogChoosePlayer, SIGNAL(clicked(QString)), this, SLOT(chooseFirstPlayerCoridor(QString)));
			connect(dialogChoosePlayer, SIGNAL(clicked(QString)), this, SLOT(sendFirstPlayerCoridorSlot(QString)));
			connect(this, SIGNAL(closeGame()), dialogChoosePlayer, SLOT(closeSlot()));
		}
		else
			qDebug() << "EROR THAT SHOULDN'T HAPPEN!!!!";
	}
	else
	{
		dialogChoosePlayer = new DialogChoosePlayer(hosting);
		connect(this, SIGNAL(closeGame()), dialogChoosePlayer, SLOT(closeSlot()));
	}
}

void MainWindow::switchToQuartoWindow(bool hosting)
{
	this->hide(); // Закрываем основное окно
	if (hosting)
	{
		if (!pLobby->connectedPlayers.empty())
		{
			dialogChoosePlayer = new DialogChoosePlayer(hosting, pLobby->host->playerName, pLobby->connectedPlayers.back()->playerName);
			connect(dialogChoosePlayer, SIGNAL(clicked(QString)), this, SLOT(chooseFirstPlayerQuarto(QString)));
			connect(dialogChoosePlayer, SIGNAL(clicked(QString)), this, SLOT(sendFirstPlayerQuartoSlot(QString)));
			connect(this, SIGNAL(closeGame()), dialogChoosePlayer, SLOT(closeSlot()));
		}
		else
			qDebug() << "EROR THAT SHOULDN'T HAPPEN!!!!";
	}
	else
	{
		dialogChoosePlayer = new DialogChoosePlayer(hosting);
		connect(this, SIGNAL(closeGame()), dialogChoosePlayer, SLOT(closeSlot()));
	}
}

void MainWindow::switchToGame(bool hosting)
{
	if (pLobby->status == InGame)
	{
		switch (pLobby->gameType)
		{
		case Coridor:
			switchToCoridorWindow(hosting);
			break;
		case Quarto:
			switchToQuartoWindow(hosting);
			break;
		}
	}
}

void MainWindow::switchToGameLikeHostSlot()
{
	if (pLobby->status == Ready)
	{
		pLobby->updateStatus(InGame);
		sendUpdateLobby(pLobby->gameType, pLobby->status);
		switchToGame(true);
	}
}

void MainWindow::returnFromGame()
{
	// deleteLobby(pLobby);
	this->show();
	switchToMain();
}

/*void MainWindow::sendQuit(QString _reciever)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::SendQuit};
	Command* pCommand = new SendQuit(_reciever);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "SendQuit Command Sent";
	this->show();
	switchToMain();
	pLobby = new Lobby();
}*/

/*void MainWindow::recieveQuit()
{
	// deleteLobby(pLobby);
}*/

void MainWindow::chooseFirstPlayerCoridor(QString _firstPlayer)
{
	QString _secondPlayer;
	if (_firstPlayer == pLobby->host->playerName)
		_secondPlayer = pLobby->connectedPlayers.back()->playerName;
	else
		_secondPlayer = pLobby->host->playerName;

	// createGame(_firstPlayer, _secondPlayer, Coridor);
	coridorWindow = new CoridorWindow(_firstPlayer, _secondPlayer, pPlayer->playerName);
	connect(coridorWindow, SIGNAL(coridorSendQPointSignal(QPoint, bool, QString, bool)), this, SLOT(coridorSendQPoint(QPoint, bool, QString, bool)));
	connect(this, SIGNAL(coridorRecieveQPointSignal(QPoint, bool, QString, bool)), coridorWindow, SLOT(coridorRecieveQPoint(QPoint, bool, QString, bool)));
	// connect(coridorWindow, SIGNAL(sendQuitSignal(QString)), this, SLOT(sendQuit(QString)));
	// connect(this, SIGNAL(sendQuitSignal()), coridorWindow, SLOT(recieveQuit()));
	connect(coridorWindow, SIGNAL(firstWindow()), this, SLOT(returnFromGame()));
	connect(this, SIGNAL(closeGame()), coridorWindow, SLOT(closeGameSlot()));
	connect(coridorWindow, SIGNAL(sendGameEndSignal()), this, SLOT(deleteLobbySlot()));
	connect(this, SIGNAL(gameEndSignal()), coridorWindow, SLOT(gameEnd()));
}

void MainWindow::chooseFirstPlayerQuarto(QString _firstPlayer)
{
	QString _secondPlayer;
	if (_firstPlayer == pLobby->host->playerName)
		_secondPlayer = pLobby->connectedPlayers.back()->playerName;
	else
		_secondPlayer = pLobby->host->playerName;

	// createGame(_firstPlayer, _secondPlayer, Quarto); sendQPointSignal
	quartoWindow = new QuartoWindow(_firstPlayer, _secondPlayer, pPlayer->playerName);
	connect(quartoWindow, SIGNAL(sendQPointSignal(QPoint, int, QString)), this, SLOT(quartoSendQPoint(QPoint, int, QString)));
	connect(this, SIGNAL(quartoRecieveQPointSignal(QPoint, int, QString)), quartoWindow, SLOT(quartoRecieveQPoint(QPoint, int, QString)));
	connect(quartoWindow, SIGNAL(quartoSendCheckWinSignal(QString, bool)), this, SLOT(quartoSendCheckWin(QString, bool)));
	connect(this, SIGNAL(quartoRecieveCheckWinSignal(QString, bool)), quartoWindow, SLOT(quartoRecieveCheckWin(QString, bool)));
	// connect(quartoWindow, SIGNAL(sendQuitSignal(QString)), this, SLOT(sendQuit(QString)));
	// connect(this, SIGNAL(sendQuitSignal()), quartoWindow, SLOT(recieveQuit()));
	connect(quartoWindow, SIGNAL(firstWindow()), this, SLOT(returnFromGame()));
	connect(this, SIGNAL(closeGame()), quartoWindow, SLOT(closeGameSlot()));
	connect(quartoWindow, SIGNAL(sendGameEndSignal()), this, SLOT(deleteLobbySlot()));
	connect(this, SIGNAL(gameEndSignal()), quartoWindow, SLOT(gameEnd()));
}

void MainWindow::chooseFirstPlayer(QString _firstPlayer, GameType _gameType)
{
	switch (_gameType)
	{
	case Coridor:
		chooseFirstPlayerCoridor(_firstPlayer);
		break;
	case Quarto:
		chooseFirstPlayerQuarto(_firstPlayer);
		break;
	default:
		qDebug() << "ChooseFirstPlayer Error";
		break;
	}
}

void MainWindow::sendFirstPlayerCoridorSlot(QString _firstPlayer)
{
	for (const auto& i : pLobby->connectedPlayers)
		sendChooseFirstPlayer(_firstPlayer, i->playerName, Coridor);
}

void MainWindow::sendFirstPlayerQuartoSlot(QString _firstPlayer)
{
	for (const auto& i : pLobby->connectedPlayers)
		sendChooseFirstPlayer(_firstPlayer, i->playerName, Quarto);
}

void MainWindow::switchToLoginIn()
{
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	connectLayout = new QGridLayout(centralWidget);
	connectBtn = new QPushButton("Connect to server");
	loginBtn = new QPushButton("Login in");
	loginEdit = new QLineEdit();
	passwordEdit = new QLineEdit();
	connectLayout->addWidget(loginEdit);
	connectLayout->addWidget(passwordEdit);
	connectLayout->addWidget(connectBtn);
	connectLayout->addWidget(loginBtn);
	connect(connectBtn, SIGNAL(clicked()), this, SLOT(connectToTheServer()));
	connect(loginBtn, SIGNAL(clicked(bool)), this, SLOT(sendLogin()));
}

void MainWindow::switchToMain()
{
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	mainLayout = new QGridLayout(centralWidget);
	createLobbyBtn = new QPushButton("Create Lobby");
	showLobbiesBtn = new QPushButton("Connect to Lobby");
	disconnectBtn = new QPushButton("Log out");

	mainLayout->addWidget(createLobbyBtn);
	mainLayout->addWidget(showLobbiesBtn);
	mainLayout->addWidget(disconnectBtn);

	connect(disconnectBtn, SIGNAL(clicked()), this, SLOT(sockDisc()));
	connect(showLobbiesBtn, SIGNAL(clicked()), this, SLOT(askLobbies()));
	connect(createLobbyBtn, SIGNAL(clicked()), this, SLOT(createLobbyDialog()));
}

void MainWindow::switchToLobby(Player* connectingPlayer, Lobby* _lobby, bool flagHosting)
{
	pLobby = _lobby;
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	lobbyLayout = new QGridLayout(centralWidget);

	connectedPlayersList = new QListWidget();

	QListWidgetItem* player1 = new QListWidgetItem("Player1", connectedPlayersList);
	player1->setCheckState(Qt::Checked);
	player1->setFlags(Qt::ItemIsEnabled); // this disables checkable...

	QListWidgetItem* tmpItem;

	gameTypeEdit = new QComboBox;
	gameTypeEdit->addItem(tr("Coridor"));
	gameTypeEdit->addItem(tr("Quarto"));
	gameTypeEdit->setCurrentIndex(pLobby->gameType);
	connect(gameTypeEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLobby(int)));

	exitLobbyBtn = new QPushButton("Exit");

	if (flagHosting)
	{
		player1->setText(pPlayer->playerName);
		// player2->setText("");

		gameTypeEdit->setEnabled(true);

		startGameBtn = new QPushButton("Start");
		connect(startGameBtn, SIGNAL(clicked()), this, SLOT(switchToGameLikeHostSlot()));
		connect(exitLobbyBtn, SIGNAL(clicked()), this, SLOT(deleteLobbySlot()));
	}
	else
	{
		if (pLobby->host->playerName == pPlayer->playerName)
		{
			player1->setText(pPlayer->playerName);

			for (const auto& i : pLobby->connectedPlayers)
			{
				tmpItem = new QListWidgetItem(i->playerName, connectedPlayersList);
				if (pLobby->status == Unready)
					tmpItem->setCheckState(Qt::Unchecked);
				else if (pLobby->status == Ready)
					tmpItem->setCheckState(Qt::Checked);
				tmpItem->setFlags(Qt::ItemIsEnabled); // this disables checkable...
			}

			gameTypeEdit->setEnabled(true);

			startGameBtn = new QPushButton("Start");
			connect(startGameBtn, SIGNAL(clicked()), this, SLOT(switchToGameLikeHostSlot()));
			connect(exitLobbyBtn, SIGNAL(clicked()), this, SLOT(deleteLobbySlot()));
		}
		else if (connectingPlayer->playerName != pPlayer->playerName)
		{
			// for already connected players (not hosting & not connecting)
		}
		else
		{
			player1->setText(pLobby->host->playerName);

			for (const auto& i : pLobby->connectedPlayers)
			{
				tmpItem = new QListWidgetItem(i->playerName, connectedPlayersList);
				if (pLobby->status == Unready)
					tmpItem->setCheckState(Qt::Unchecked);
				else if (pLobby->status == Ready)
					tmpItem->setCheckState(Qt::Checked);
				tmpItem->setFlags(Qt::ItemIsEnabled); // this disables checkable...
			}

			gameTypeEdit->setEnabled(false);

			startGameBtn = new QPushButton("Ready");
			connect(exitLobbyBtn, SIGNAL(clicked()), this, SLOT(leaveLobby()));
			connect(startGameBtn, SIGNAL(clicked()), this, SLOT(sendRdy()));
		}
	}

	lobbyLayout->addWidget(connectedPlayersList);
	lobbyLayout->addWidget(gameTypeEdit);
	lobbyLayout->addWidget(startGameBtn);
	lobbyLayout->addWidget(exitLobbyBtn);
}

void MainWindow::switchToLobbiesList()
{
	centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	lobbiesListLayout = new QGridLayout(centralWidget);

	lobbiesList = new QTreeWidget();
	lobbiesList->setColumnCount(5);
	QStringList headers;
	headers << tr("Lobby Name") << tr("Host") << tr("Game Type") << tr("Players Number") << tr("Status");
	lobbiesList->setHeaderLabels(headers);
	updateLobbiesListBtn = new QPushButton("Update");
	exitLobbiesBtn = new QPushButton("Exit");

	lobbiesListLayout->addWidget(lobbiesList);
	lobbiesListLayout->addWidget(updateLobbiesListBtn);
	lobbiesListLayout->addWidget(exitLobbiesBtn);

	connect(updateLobbiesListBtn, SIGNAL(clicked()), this, SLOT(askLobbies()));
	connect(exitLobbiesBtn, SIGNAL(clicked()), this, SLOT(leaveLobbiesList()));
}

void MainWindow::sockDisc()
{
	qDebug() << "sockDisconnected";
	pSocket->deleteLater();
	emit closeGame();
	this->show();
	switchToLoginIn();
}

void MainWindow::sendCreateLobby(QString lobbyName, QString hostLogin, int gameType)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::CreateLobby};
	Command* pCommand = new CreateLobby(new Lobby(lobbyName, hostLogin, gameType));

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "CreateLobby Command Sent";
}

void MainWindow::deleteLobby(Lobby* lobby)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::DeleteLobby};
	Command* pCommand = new DeleteLobby(lobby);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "DeleteLobby Command Sent";
	// pLobby = new Lobby();
}

void MainWindow::sendConnectToLobby(Lobby* _lobby, Player* _player, bool _connectFlag)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::ConnectToLobby};

	Command* pCommand = new ConnectToLobby(_lobby, _player, _connectFlag);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "ConnectToLobby Command Sent" << _connectFlag;
}

void MainWindow::sendUpdateLobby(int _gameType, int _status)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::UpdateLobby};
	Command* pCommand = new UpdateLobby(_gameType, _status, pLobby->connectedPlayers);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "ChangeGameType Command Sent";
}

void MainWindow::sendChooseFirstPlayer(QString _firstPlayer, QString _guest, GameType _gameType)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::SendFirstPlayer};
	Command* pCommand = new SendFirstPlayer(_firstPlayer, _guest, _gameType);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "CreateLobby Command Sent";
}

void MainWindow::coridorSendQPoint(QPoint point, bool move, QString enemy, bool horizontal)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::CoridorSendQPoint};
	Command* pCommand = new CoridorSendQPoint(point, move, enemy, horizontal);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "CoridorSendQPoint Command Sent";
}

void MainWindow::quartoSendQPoint(QPoint point, int figureId, QString enemy)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::QuartoSendQPoint};
	Command* pCommand = new QuartoSendQPoint(point, figureId, enemy);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "QuartoSendQPoint Command Sent";
}

void MainWindow::quartoSendCheckWin(QString enemy, bool checkWin)
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::QuartoSendCheckWin};
	Command* pCommand = new QuartoSendCheckWin(enemy, checkWin);

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "QuartoSendCheckWin Command Sent";
}

void MainWindow::askLobbies()
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_9);

	CommandType commandType = {CommandType::Type::AskLobbies};
	Command* pCommand = new AskLobbies();

	out << commandType;
	pCommand->operator<<(out);
	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	qDebug() << "AskLobbies Command Sent";
}

void MainWindow::updateLobby(int _gameType)
{
	// pLobby->update(_gameType, Unready);
	sendUpdateLobby(_gameType, Unready);
}

void MainWindow::sockReady()
{
	/*if (pLobby->connectedPlayers.empty())
		qDebug() << "11111111111111111111111111111";*/

	CommandFactory factory;
	QDataStream in(pSocket);
	in.setVersion(QDataStream::Qt_5_9);
	pCommand = factory.create(in);
	pCommand->execute();
	switchCmd();

	/*if (pLobby->connectedPlayers.empty())
		qDebug() << "222222222222222222222222222222222222222222222222";*/
}

/*void MainWindow::sendMessage()
{
	QByteArray arrBlock;
	QDataStream out(&arrBlock, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_10);

	CommandType commandType = {CommandType::Type::AskLogin};
	Command* pCommand = new Login(ui->lineEdit->text());

	out << commandType;
	pCommand->operator<<(out);

	pSocket->write(arrBlock);
	pSocket->waitForBytesWritten();

	// lineEdit->setText("");

	qDebug() << "Message Sent";
}*/

void MainWindow::switchCmd()
{
	if (Login* pLogin = dynamic_cast<Login*>(pCommand))
	{
		pPlayer = pLogin->player;
		switchToMain();
	}
	else if (CreateLobby* pCreateLobby = dynamic_cast<CreateLobby*>(pCommand))
	{
		pLobby = pCreateLobby->lobby;
		switchToLobby(new Player(), pLobby, true);
	}
	else if (SendLobbies* pSendLobbies = dynamic_cast<SendLobbies*>(pCommand))
	{
		switchToLobbiesList();

		// list<QTreeWidgetItem*> lobbyItems;
		QTreeWidgetItem* tmpItem;
		for (const auto& i : pSendLobbies->lobbies)
		{
			tmpItem = new QTreeWidgetItem(lobbiesList);
			tmpItem->setText(0, i->lobbyName);
			tmpItem->setText(1, i->host->playerName);
			tmpItem->setText(2, i->gameTypeStr);
			tmpItem->setText(3, QString::number(i->connectedPlayersNumber) + "/" + QString::number(i->maxPlayers));
			tmpItem->setText(4, i->statusStr);
		}
		connect(lobbiesList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(sendConnectToLobbySlot(QTreeWidgetItem*, int)));
	}
	else if (DeleteLobby* pDeleteLobby = dynamic_cast<DeleteLobby*>(pCommand))
	{
		if (pLobby->gameType != WrongGameType)
			switchToMain();
		emit gameEndSignal();
		pLobby = new Lobby();
	}
	else if (UpdateLobby* pUpdateLobby = dynamic_cast<UpdateLobby*>(pCommand))
	{
		pLobby->update(pUpdateLobby->gameType, pUpdateLobby->status);
		switchToLobby(new Player(), pLobby, false);
		if (pUpdateLobby->status != InGame)
		{
			for (const auto& i : pLobby->connectedPlayers)
				if (i->playerName == pPlayer->playerName)
					switchToLobby(i, pLobby, false);
		}
		else if (pUpdateLobby->gameType == WrongGameType)
		{
			// close current game
			returnFromGame();
			QMessageBox::information(this, tr("Error"), "Oponent has left the game");
		}
		else
			switchToGame(false);
	}
	else if (ConnectToLobby* pConnectToLobby = dynamic_cast<ConnectToLobby*>(pCommand))
	{
		if (pConnectToLobby->connectFlag)
			//***** connect
			switchToLobby(pConnectToLobby->player, pConnectToLobby->lobby, false);
		else
		{
			//***** disconnect
			switchToLobby(pConnectToLobby->player, pConnectToLobby->lobby, false);
		}
	}
	else if (SendRdy* pSendRdy = dynamic_cast<SendRdy*>(pCommand))
	{
		setRdy();
	}
	else if (SendMessage* pSendMessage = dynamic_cast<SendMessage*>(pCommand))
	{
		if (pSendMessage->errorFlag)
			QMessageBox::information(this, tr("Error"), pSendMessage->message);
	}
	else if (SendFirstPlayer* pSendFirstPlayer = dynamic_cast<SendFirstPlayer*>(pCommand))
	{
		dialogChoosePlayer->close();
		chooseFirstPlayer(pSendFirstPlayer->firstPlayer, (GameType)pSendFirstPlayer->gameType);
	}
	/*else if (SendQuit* pSendQuit = dynamic_cast<SendQuit*>(pCommand))
	{
		// emit sendQuitSignal();
	}*/
	else if (CoridorSendQPoint* pCoridorSendQPoint = dynamic_cast<CoridorSendQPoint*>(pCommand))
	{
		emit coridorRecieveQPointSignal(pCoridorSendQPoint->point, pCoridorSendQPoint->move, pCoridorSendQPoint->enemy, pCoridorSendQPoint->horizontal);
	}
	else if (QuartoSendQPoint* pQuartoSendQPoint = dynamic_cast<QuartoSendQPoint*>(pCommand))
	{
		emit quartoRecieveQPointSignal(pQuartoSendQPoint->point, pQuartoSendQPoint->figureId, pQuartoSendQPoint->enemy);
	}
	else if (QuartoSendCheckWin* pQuartoSendCheckWin = dynamic_cast<QuartoSendCheckWin*>(pCommand))
	{
		emit quartoRecieveCheckWinSignal(pQuartoSendCheckWin->enemy, pQuartoSendCheckWin->checkWin);
	}
}
