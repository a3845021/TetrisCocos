#include "CricketGameOverScene.h"
#include "TetrisButton.h"
#include "TetrisCricketBoardScene.h"
#include "MainMenuScene.h"


USING_NS_CC;

CricketGameOverScene * CricketGameOverScene::create(int score, std::string winPlayerName)
{
	auto scene = new(std::nothrow)CricketGameOverScene(score, winPlayerName);
	if (scene && scene->init())
	{
		scene->autorelease();
	}
	else
	{
		delete scene;
		scene = nullptr;
	}
	return scene;
}

bool CricketGameOverScene::init()
{
	if (!Scene::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();

	// draw heading
	auto txtDrawNode = DrawNode::create();
	addChild(txtDrawNode);

	addChild(TetrisFont::create("game over", txtDrawNode, cocos2d::Color4F::GRAY, Vec2(visibleSize.width * 0.5, 0.9 * visibleSize.height), 2.2f, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, FontAlign::MIDDLE));

	// draw custom message
	std::string text;
	if (score == 0)
		text = "tough draw";
	else
	{
		text = winPlayerName;
		text.append(" won by ");
		text.append(std::to_string(score));
		text.append(" runs");
	}
	addChild(TetrisFont::create(text, txtDrawNode, cocos2d::Color4F::GRAY, Vec2(visibleSize.width * 0.5, 0.63 * visibleSize.height), 0.5f, FontColorPattern::FULL, FontDrawPattern::SOLID, FontAlign::MIDDLE));


	addChild(TetrisButton::create([&](cocos2d::Ref*) {
		Director::getInstance()->replaceScene(TetrisCricketBoardScene::create());
	}, "rematch", Color4F::RED, Vec2(visibleSize.width * 0.5, visibleSize.height * 0.40), 1.5, FontAlign::MIDDLE, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, 7));

	addChild(TetrisButton::create([](cocos2d::Ref*) {
		Director::getInstance()->replaceScene(MainMenuScene::create());
	}, "menu", Color4F::RED, Vec2(visibleSize.width * 0.5, visibleSize.height * 0.25), 1.5, FontAlign::MIDDLE, FontColorPattern::RANDOM_BLOCK, FontDrawPattern::SOLID, 7));

	return true;
}