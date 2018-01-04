#include "UnitBlock.h"
#include "Constants.h"

USING_NS_CC;

double UnitBlock::_u = 0;
Vec2 UnitBlock::_pf = Vec2();

//UnitBlock::UnitBlock()
//{
//}
//
//UnitBlock::~UnitBlock()
//{
//}

UnitBlock * UnitBlock::create(int x, int y, cocos2d::Color4B color)
{
	UnitBlock* block = new(std::nothrow)UnitBlock();
	if (block && block->init(x, y, color))
	{
		block->autorelease();
		return block;
	}
	else
	{
		delete block;
		block = nullptr;
		return nullptr;
	}
}

bool UnitBlock::init(int x, int y, cocos2d::Color4B color)
{
	if (!Node::init())
	{
		return false;
	}

	this->_x = x;
	this->_y = y;
	this->_color = color;

	_drawNode = DrawNode::create();
	this->addChild(_drawNode);

	return true;
}

void UnitBlock::drawBlock()
{
	Vec2 midPoint(_pf.x + _x * _u, _pf.y - _y * _u);
	Vec2 origin(midPoint.x - _u / 2, midPoint.y - _u / 2);
	Vec2 destination(midPoint.x + _u / 2, midPoint.y + _u / 2);

	_drawNode->clear();
	//_drawNode->drawRect(origin, destination, this->_color);
	_drawNode->drawSolidRect(origin, destination, cocos2d::Color4F(this->_color));
}

void UnitBlock::moveDown()
{
	this->_y++;
	this->drawBlock();
}

void UnitBlock::moveLeft()
{
	this->_x--;
	this->drawBlock();
}

void UnitBlock::moveRight()
{
	this->_x++;
	this->drawBlock();
}

void UnitBlock::moveAt(BoardPos pos)
{
	this->_x = pos.x;
	this->_y = pos.y;
	drawBlock();
}


bool UnitBlock::checkMoveAt(BoardPos nextPos, const SolidBlocks& solidBlocks)
{
	if (solidBlocks.find(nextPos) == true
		|| nextPos.x < Constant::BUCKET_LEFT || nextPos.x > Constant::BUCKET_RIGHT - 1
		|| nextPos.y < Constant::BUCKET_TOP || nextPos.y > Constant::BUCKET_BOTTOM - 1)
	{
		// block is either touching something or has hit the boundraies
		return false;
	}

	return true;
}

bool UnitBlock::checkMoveDown(const SolidBlocks& solidBlocks)
{
	BoardPos nextPos(_x, _y + 1); // next position down
	if (solidBlocks.find(nextPos) == true
		|| nextPos.y > Constant::BUCKET_BOTTOM - 1)
	{
		// block is either touching something or has hit the boundraies
		return false;
	}
	return true;
}

bool UnitBlock::checkMoveLeft(const SolidBlocks& solidBlocks)
{
	BoardPos nextPos(_x - 1, _y); // next position left
	if (solidBlocks.find(nextPos) == true
		|| nextPos.x < Constant::BUCKET_LEFT)
	{
		// block is either touching something or has hit the boundraies
		return false;
	}
	return true;
}

bool UnitBlock::checkMoveRight(const SolidBlocks& solidBlocks)
{
	BoardPos nextPos(_x + 1, _y); // next position right
	if (solidBlocks.find(nextPos) == true
		|| nextPos.x > Constant::BUCKET_RIGHT - 1)
	{
		// block is either touching something or has hit the boundraies
		return false;
	}
	return true;
}

