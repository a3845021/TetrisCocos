#include "TetrisBoardScene.h"
#include "Constants.h"
#include <algorithm>
#include <cstdlib>
#include "TetrisFont.h"

USING_NS_CC;


bool TetrisBoardScene::init()
{
	// super init
	if (!GameScene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	//visibleSize.width /= 1.5;
	this->calcSceneDrawingData(_u, _pf, visibleSize, t_const::sp::NUM_OF_UNIT_BLOCKS_IN_HEIGHT, t_const::sp::NUM_OF_UNIT_BLOCKS_IN_WIDTH);

	// set up keyboard event listner
	auto eventListner = EventListenerKeyboard::create();
	eventListner->onKeyPressed = CC_CALLBACK_2(TetrisBoardScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListner, this);

	// initialize with four values
	srand(time(NULL));
	for (short i = 0; i < 4; i++)
	{
		randList.push_back(rand() % TetrominoTemplate::size);
	}
	randListIter = randList.begin();

	initFromDB();
	int level = 1;
	// add board
	board = Board::createBoard(_u, _pf, randListIter, highScore, level, t_const::sp::BUCKET_LEFT, t_const::sp::BUCKET_RIGHT, t_const::sp::BUCKET_TOP, t_const::sp::BUCKET_BOTTOM, t_const::sp::SPAWN_POSITION);
	board->registerObserver(this);
	this->addChild(board);

	windowDrawNode = DrawNode::create();
	this->addChild(windowDrawNode);
	this->drawWindow();
	drawFonts(visibleSize);

	countDownLayer = nullptr;
	countDown(visibleSize);

	return true;
}


void TetrisBoardScene::start()
{
	this->addButtons();
	this->removeChild(countDownLayer);
	addText();
	board->start();
}


void TetrisBoardScene::initFromDB()
{
	// get values from db
	highScore = 0;
	char *errorMsg;

	// open db
	if (sqlite3_open("tetris.db", &db))
	{
		cocos2d::log("can't open single_player_db database");
		cocos2d::log(sqlite3_errmsg(db));
	}
	else
	{
		cocos2d::log("single_player_db opend...");

		// get data from single_player_table
		char *query = "select score from sp where name = 'player';";

		int rc = sqlite3_exec(db, query, [](void *data, int argc, char **argv, char **azColName) -> int {
			int *d = (int*)data;
			*d = std::atoi(argv[0]);

			return 0;
		}
		, (void*)&highScore, &errorMsg);

		if (rc != SQLITE_OK)
		{
			cocos2d::log("sql error");
			cocos2d::log(errorMsg);
			sqlite3_free(errorMsg);
		}
		else
		{
			cocos2d::log("highscore loaded succefully");
		}

	}
	sqlite3_close(db);
}


void TetrisBoardScene::saveToDB()
{
	char *errorMsg;

	// save highScore to db
	if (sqlite3_open("tetris.db", &db))
	{
		cocos2d::log("can't open db");
		cocos2d::log(sqlite3_errmsg(db));
	}
	else
	{
		cocos2d::log("db opend...");
		// add data to single_player_table
		std::string query = "update sp set score = ";
		query.append(std::to_string(highScore));
		query.append(", max_level = ");
		query.append(std::to_string(maxLevel));
		query.append(" where name = 'player' ;");

		int rc = sqlite3_exec(db, query.c_str(), NULL, 0, &errorMsg);
		if (rc != SQLITE_OK)
		{
			cocos2d::log("can't update db");
			cocos2d::log(errorMsg);
			sqlite3_free(errorMsg);
		}
		else
		{
			cocos2d::log("score updated in db");
		}
	}
	sqlite3_close(db);
}


void TetrisBoardScene::onNotify(const Board & board, TetrisEvent _event)
{
	switch (_event)
	{
	case INCREMENT_RAND_ITERATOR:

		++randListIter;
		randList.push_back(rand() % TetrominoTemplate::size);
		randList.pop_front();
		redrawWindow();

		break;
	case GAMEOVER:
		// save game highScore
		if (board.getScore() >= board.getHighScore())
		{
			this->highScore = board.getScore();
			this->maxLevel = board.getLevel();
			saveToDB();
		}

		// display game over scene
		this->GoToGameOverScene(this);

		break;
	case LEVEL_UP:
		lvlNum->reWrite(std::to_string(board.getLevel()), lvlNumDrawNode);
		break;
	case HIGH_SCORE_UP:
		hScoreNum->reWrite(std::to_string(board.getHighScore()), hScoreNumDrawNode);
		break;
	case SCORE_UP:
		scoreNum->reWrite(std::to_string(board.getScore()), scoreNumDrawNode);
		break;
	default:
		break;
	}

}


void TetrisBoardScene::drawFonts(Size visibleSize)
{
	addChild(txtDrawNode = DrawNode::create());
	addChild(scoreNumDrawNode = DrawNode::create());
	addChild(hScoreNumDrawNode = DrawNode::create());
	addChild(lvlNumDrawNode = DrawNode::create());

	addChild(scoreTxt = TetrisFont::create("score", txtDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT - 1), 0.4 * visibleSize.height), 0.5f, FontColorPattern::FULL, FontDrawPattern::SOLID, FontAlign::RIGHT));
	addChild(hScoreTxt = TetrisFont::create("h score", txtDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT - 1), 0.3 * visibleSize.height), 0.5f, FontColorPattern::FULL, FontDrawPattern::SOLID, FontAlign::RIGHT));
	addChild(lvlTxt = TetrisFont::create("level", txtDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT - 1), 0.2 * visibleSize.height), 0.5f, FontColorPattern::FULL, FontDrawPattern::SOLID, FontAlign::RIGHT));

	addChild(scoreNum = TetrisFont::create(".", scoreNumDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT), 0.4 * visibleSize.height), 0.6f, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, FontAlign::LEFT));
	std::string hsTxt(".");
	if (this->highScore > 0)
		hsTxt = std::to_string(this->highScore);
	addChild(hScoreNum = TetrisFont::create(hsTxt, hScoreNumDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT), 0.3 * visibleSize.height), 0.6f, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, FontAlign::LEFT));
	addChild(lvlNum = TetrisFont::create("1", lvlNumDrawNode, cocos2d::Color4F::GRAY,
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT), 0.2 * visibleSize.height), 0.6f, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, FontAlign::LEFT));
}


void TetrisBoardScene::drawWindow()
{
	auto drawNode = DrawNode::create();
	this->addChild(drawNode);

	drawNode->drawRect(
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_LEFT) - _u / 2, _pf.y - _u * (t_const::sp::WINDOW_TOP)),
		Vec2(_pf.x + _u * (t_const::sp::WINDOW_RIGHT), _pf.y - _u * (t_const::sp::WINDOW_BOTTOM)),
		Color4F::ORANGE
	);
	redrawWindow();
}

void TetrisBoardScene::redrawWindow()
{
	windowDrawNode->clear();
	int i = 0;
	for (auto iter = ++randList.begin(); iter != randList.end(); ++iter)
	{
		auto tet = Tetromino::create(_u, _pf,
			TetrominoTemplate::rotationTemplates->at(*iter)->getInitialRotation(),
			TetrominoTemplate::colorTemplates->at(*iter),
			TetrominoTemplate::borderColorTemplates->at(*iter),
			BoardPos(t_const::sp::WINDOW_LEFT + 1, (t_const::NUM_OF_UNIT_BLOCKS_IN_TETROMINO * i) + t_const::sp::WINDOW_TOP + 1)
		);

		tet->draw(windowDrawNode);

		++i;
	}
}


void TetrisBoardScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event)
{

	switch (keyCode)
	{

	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:

		board->movingBlockDown();
		down->freeze();

		break;

	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:

		board->movingBlockRight();
		right->freeze();

		break;

	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:

		board->movingBlockLeft();
		left->freeze();

		break;

	case EventKeyboard::KeyCode::KEY_UP_ARROW:

		board->movingBlockRotate();
		up->freeze();

		break;

	case EventKeyboard::KeyCode::KEY_SPACE:

		board->movingBlockGravityDrop();
		gdrop->freeze();

		break;

	default: break;

	}

}


void TetrisBoardScene::addText()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// player controls
	auto x = 0.12 * visibleSize.width;
	auto y = visibleSize.height * 0.4;

	addChild(up = TetrisButton::create("#", Vec2(x, y), 1.0f));
	addChild(down = TetrisButton::create("@", Vec2(x, y - 2 * _u), 1.0f));
	addChild(gdrop = TetrisButton::create("[==]", Vec2(x, y - 4 * _u), 1.0f));
	addChild(right = TetrisButton::create("$", Vec2(x + 2 * _u, y - _u), 1.0f));
	addChild(left = TetrisButton::create("!", Vec2(x - 2 * _u, y - _u), 1.0f));

	up->drawBorder();
	down->drawBorder();
	gdrop->drawBorder();
	right->drawBorder();
	left->drawBorder();

	// arrow
	addChild(TetrisFont::create("!", txtDrawNode, Color4F::GRAY, Vec2(_pf.x + (t_const::sp::WINDOW_RIGHT + 1)* _u, visibleSize.height - 2 * _u), 1.0f, FontColorPattern::FULL, FontDrawPattern::SOLID));
}