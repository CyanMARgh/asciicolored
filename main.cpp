#define PREPARE
#define RUN

#ifdef PREPARE
#include <SFML/Graphics.hpp>
#endif
#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>
#include <thread>

const int framecount = 5301, framedelay = 1, timemcs = 213000000;
const int sizex = 130, sizey = 40;
const std::string megafile = "frames";

void showcursor(int signum) {
	printf("\e[?25h");
	exit(signum);
}
#ifdef PREPARE
sf::Image resize(const sf::Image& img, sf::Vector2u s_) {
	sf::Image img_;
	sf::Vector2u s = img.getSize();
	img_.create(s_.x, s_.y);
	for (uint32_t y_ = 0; y_ < s_.y; y_++) {
		for (uint32_t x_ = 0; x_ < s_.x; x_++) {
			auto x = (uint32_t)((double)x_ / s_.x * s.x);
			auto y = (uint32_t)((double)y_ / s_.y * s.y);

			sf::Color c = img.getPixel(x, y);
			img_.setPixel(x_, y_, c);
		}
	}
	return img_;
}
inline void printcolorspace(sf::Color c) {
	printf("\e[48;2;%d;%d;%dm ", c.r, c.g, c.b);
}
void printASCII(const sf::Image& img) {
	auto s = img.getSize();
	auto pptr = (sf::Color*)img.getPixelsPtr();
	for (int y = 0, i = 0; y < s.y; y++) {
		for (int x = 0; x < s.x; x++, i++) {
			printcolorspace(pptr[i]);
		}
		puts("\e[m");
	}
}
void saveAsText(const sf::Image& img, std::ofstream& fout) {
	auto s = img.getSize();
	auto pptr = (sf::Color*)img.getPixelsPtr();

	for (int y = 0, i = 0; y < s.y; y++) {
		for (int x = 0; x < s.x; x++, i++) {
			auto c = pptr[i];
			fout << ("\033[48;2;" + std::to_string(c.r) + ";" + std::to_string(c.g) + ";" + std::to_string(c.b) + "m ");
		}
		fout << "\e[m\n";
	}
}
void prepare() {
	sf::Image img;
	std::ofstream fout(megafile);
	for (int i = 0; i < framecount; i++) {
		img.loadFromFile("result/frame" + std::to_string(i * framedelay) + ".png");
		saveAsText(resize(img, sf::Vector2u(sizex, sizey)), fout);
		if ((i + 1) % 10 == 0 || i + 1 == framecount) {
			printf("%4d/%4d\n\e[1F", i + 1, framecount);
		}
	}
}
#endif
#ifdef RUN
inline void resetcursor(int Y) {
	printf("\e[%dA", Y);
}
void printNLinesFromFile(std::ifstream& fin, int linesmax) {
	std::stringstream str;
	int line = 0;
	if (fin.is_open()) {
		while (line < linesmax && fin.peek() != EOF) {
			char c = fin.get();
			if (c == '\n') line++;
			str << (char)c;
		}
		printf("%s", str.str().c_str());
	}
}
void run() {
	signal(SIGINT, showcursor);
	signal(SIGTSTP, showcursor);
	std::ifstream fin(megafile);

	printf("\e[?25l");
	for (int i = 0; i < framecount; i++) {
		printNLinesFromFile(fin, sizey);

		if (i != framecount - 1)resetcursor(sizey);
		std::this_thread::sleep_for(std::chrono::microseconds(timemcs / framecount));
	}
	showcursor(0);
}
#endif
int main() {
#ifdef PREPARE
	prepare();
#endif
#ifdef RUN
	run();
#endif
	return 0;
}
