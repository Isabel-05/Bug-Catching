#include "Bug.h"
#include <random>
#include <cmath>

const std::string LADYBUGPATH = "./Images/ladybug-rotated.bmp";
const std::string MOTHPATH = "./Images/moth.bmp";
const std::string BUTTERFLYPATH = "./Images/butterfly.bmp";

std::random_device randomDevice;
std::mt19937 generator(randomDevice());

void Bug::init(SDL_Renderer* renderer, BugType inType, glm::vec2 inPosition, glm::vec2 inForward)
{
	position = inPosition;
	forward = inForward;
	bugType = inType;

	//1080 is the resolution i balanced the movement speed around so im scaling it proportionally so its the same on other resolutions
	float wScalar = WINDOWSIZE / 1080.0f;

	switch (inType)
	{
	case BugType::LADYBUG:
		sprite.init(renderer, LADYBUGPATH);
		movementSpeed = 180 * wScalar;
		lifeTime = 2;
		pointsAwarded = 1;
		break;
	case BugType::MOTH:
		sprite.init(renderer, MOTHPATH);
		movementSpeed = 300 * wScalar;
		lifeTime = 4;
		pointsAwarded = 2;
		break;
	case BugType::BUTTERFLY:
		sprite.init(renderer, BUTTERFLYPATH);
		movementSpeed = 400 * wScalar;
		lifeTime = 5;
		pointsAwarded = 3;
		break;
	}

	resetTimer();
}

void Bug::initRandom(SDL_Renderer* renderer)
{
	std::uniform_int_distribution<int> genPos(1, WINDOWSIZE); //1 to prevent glm::normalize(0,0)
	std::uniform_int_distribution<int> genSign(0, 1);
	std::uniform_int_distribution<int> genType(0, 6);

	//random spawn position on one of the edges (off screen)
	glm::vec2 randPos;

	int edge_axis = genSign(generator); //determines if its one of the horizontal or vertical edges
	int edge_offset = genSign(generator); //determines wether it spawns at the near edge (0) or the far edge (windowsize)
	int bug_pos = genPos(generator); //determines where along that edge it spawns

	int offset = edge_offset * WINDOWSIZE;
	int sign = edge_offset == 1 ? 1 : -1;

	if (edge_axis == 0)
	{
		randPos.x = offset + sign * SPAWNMARGIN;
		randPos.y = bug_pos;
	}
	else
	{
		randPos.x = bug_pos;
		randPos.y = offset + sign * SPAWNMARGIN; 
	}

	//Random forward vector (pointing at a point on screen)
	glm::vec2 randForward = glm::normalize(glm::vec2(genPos(generator), genPos(generator)) - randPos);

	//Random Type
	//Type chances roughly (ladybug: 57%, moth: 29%, Butterfly: 14%)
	BugType randType = BugType::LADYBUG;
	int codedType = genType(generator);

	if (codedType <= 3) {
		randType = BugType::LADYBUG;
	}
	else if (codedType <= 5) {
		randType = BugType::MOTH;
	}
	else {
		randType = BugType::BUTTERFLY;
	}

	init(renderer, randType, randPos, randForward);
}

void Bug::update(float deltaTime)
{
	changeDirTimer -= deltaTime;

	if (changeDirTimer < 0.0f && lifeTime > 0)
	{
		std::uniform_int_distribution<int> genPos(0, WINDOWSIZE);

		forward = glm::normalize(glm::vec2(genPos(generator), genPos(generator)) - position);
		resetTimer();
		lifeTime-- ;
	}
	
	position.x += movementSpeed * forward.x * deltaTime;
	position.y += movementSpeed * forward.y * deltaTime;
}

void Bug::renderSprite(SDL_Renderer* renderer)
{
	SDL_FRect rect{};
	rect.x = position.x - (SPRITESIZE * 0.5f);
	rect.y = position.y - (SPRITESIZE * 0.5f);
	rect.w = SPRITESIZE;
	rect.h = SPRITESIZE;

	if (bugType == BugType::BUTTERFLY)
	{
		sprite.renderRotated(renderer, rect, 0.0, forward.x < 0);
		return;
	}

	//the ladybug and moth sprite face directly up
	//that means i cant just get the angle; since atan goes from (1,0) counter clockwise
	//i add 90 degrees to line it up with the sprites
	double angle = std::atan2(forward.y, forward.x);
	angle =  angle * (180.0 / 3.14159265358979) + 90; //convert to degrees and add offset

	sprite.renderRotated(renderer, rect, angle, false);
}

void Bug::resetTimer()
{
	// i used if/else instead of switch here since the compiler doesnt want to risk double initialization (even though its not technically possible)
	if (bugType == BugType::LADYBUG) {
		std::uniform_real_distribution<float> genTimer(1.8f, 3.2f);
		changeDirTimer = genTimer(generator);
	}
	else if (bugType == BugType::MOTH) {
		std::uniform_real_distribution<float> genTimer(0.8f, 1.8f);
		changeDirTimer = genTimer(generator);
	}
	else if (bugType == BugType::BUTTERFLY) {
		std::uniform_real_distribution<float> genTimer(0.2f, 0.7f);
		changeDirTimer = genTimer(generator);
	}
}
