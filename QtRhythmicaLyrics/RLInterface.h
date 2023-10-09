#pragma once
#include <QFont>
#include <QColor>
#include <QPolygon> 
#include <QMediaPlayer> 
#include <QAudioOutput>
#include <QTime>
#include <QRegExp>
#include <QFile>



using namespace std;
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

class RLInterface {
};

enum class WORDTYPE {
	normal = 0,
	alphabet,
	blank
};
struct struLrc {
	WORDTYPE type;
	QString word;
	QString ruby;
	vector<QTime> timetags;
	int checkCnt;
};

namespace Lyrics {
	static QString title, artist, album, tagger;
	static QString lrc_words;
	static QString lrc_output;
	static vector<vector<struLrc>> lrcs;

	static void LrcInput(QUrl url) {
		QFile* fp = new QFile(url.toLocalFile());
		if (!fp->open(QIODevice::ReadOnly)) return;

		int row, col;
		row = 0;
		while (1) {
			QString str = fp->readLine();
			if (str.isEmpty()) break;

			auto it = str.end();
			it--; it--;
			str.erase(it, str.end());
			if (str.isEmpty()) continue;

			QRegExp reg;
			// title
			reg.setPattern("\\[ti:(.*)\\]");
			if (reg.indexIn(str) != -1) {
				title = reg.cap(1);
				continue;
			}
			// artist
			reg.setPattern("\\[ar:(.*)\\]");
			if (reg.indexIn(str) != -1) {
				artist = reg.cap(1);
				continue;
			}
			// album
			reg.setPattern("\\[al:(.*)\\]");
			if (reg.indexIn(str) != -1) {
				album = reg.cap(1);
				continue;
			}

			col = 0;
			reg.setPattern("\\[(\\d+):(\\d+)[:\\.](\\d+)\\]");
			QString tmp;
			QTime t;
			for (size_t i = 0; i < str.size(); i++) {
				if (str[i] == '[' || !tmp.isEmpty()) {
					tmp += str[i];
					if (str[i] == ']') {
						if (reg.indexIn(tmp) != -1) {
							int min = reg.cap(1).toInt();
							int sec = reg.cap(2).toInt();
							int msec = reg.cap(3).toInt();
							t.setHMS(0, min, sec, msec);
							if (lrcs[row][col].timetags.size() > 0) {
								lrcs[row][col].timetags.clear();
							}
							lrcs[row][col].timetags.push_back(t);
						}
						tmp.clear();
					}
					continue;
				}
				lrcs[row][col].word += str[i];
				col++;
			}
			row++;
		}
	}
}


struct struPaintCfg {
	QFont ftMain;		//主文字字体
	QFont ftRuby;		//注音字体
	QColor clBG;		//背景颜色
	QColor clMain;		//主要颜色（常亮/切换后）
	QColor clWarp;		//文字颜色（切换前）
	QColor clTimetag;	//timetag颜色
	QColor clCheck;		//check颜色
	QColor clCurr;		//当前check颜色
	QColor clArrow;		//当前位置指示箭头颜色
};

namespace PaintCfg {
	static struPaintCfg cfg;

	static void setFont(const QFont& font) {
		cfg.ftMain = font;
	}
	static void setFontRuby(const QFont& font) {
		cfg.ftRuby = font;
	}
	static void setclBG(const QColor& color) {
		cfg.clBG = color;
	}
	static void setclMain(const QColor& color) {
		cfg.clMain = color;
	}
	static void setclWarp(const QColor& color) {
		cfg.clWarp = color;
	}
	static void setclTimetag(const QColor& color) {
		cfg.clTimetag = color;
	}
	static void setclCheck(const QColor& color) {
		cfg.clCheck = color;
	}
	static void setclCurr(const QColor& color) {
		cfg.clCurr = color;
	}
	static void setclArrow(const QColor& color) {
		cfg.clArrow = color;
	}
	static void setPaintCfg(const struPaintCfg& cfg) {
		PaintCfg::cfg = cfg;
	}

	//////////////////////////////////////////////////////////////////////////
	static int uniLen;
	static QPolygon polyArrow;

	static QPolygon polyTimetag;
	static QList<QPolygon> polyChecks;
	static QSize rcSize;
	static void InitPolyCfg() {
		uniLen = cfg.ftMain.pixelSize() / 20;
		int len = uniLen;

		rcSize.setWidth(len * 18);
		rcSize.setHeight(len * 11);

		QList<QPoint> pointsArrow = {
			{0, 6 * len},
			{2 * len, 6 * len},
			{2 * len, 11 * len},
			{3 * len, 11 * len},
			{3 * len, len }
		};
		polyArrow.append(pointsArrow);


		QList<QPoint> ptsPolyTimetag = {
			{len, 6 * len},
			{len, 10 * len},
			{5 * len, 10 * len}
		};
		polyTimetag.append(ptsPolyTimetag);


		for (size_t i = 0; i < 7; i++) {
			polyChecks.append(QPolygon());
		}

		QList<QPoint> ptsCheck0 = {
			{len, len},
			{len, 5 * len},
			{5 * len, 5 * len}
		};
		polyChecks[0].append(ptsCheck0);

		QList<QPoint> checkPos = {
			{len,len},
			{6 * len,len},
			{9 * len,len},
			{12 * len,len},
			{6 * len,6 * len},
			{9 * len,6 * len},
			{12 * len,6 * len}
		};
		for (size_t i = 1; i < 7; i++) {
			int x = checkPos[i].x();
			int y = checkPos[i].y();
			QList<QPoint> ptsCheck = {
				{x,y},
				{x,y + 4 * len},
				{x + 2 * len,y + 4 * len},
				{x + 2 * len,y }
			};
			polyChecks[i].append(ptsCheck);
		}
	}
}

namespace Player {
	static QMediaPlayer* player;
	static QAudioOutput* audioOutput;
	static QString musicName;

	static float bwdTime;	//回退时间，sec
	static float fwdTime;	//快进时间，sec

	static unordered_set<QString> musicFile{
		"mp3", "mp4", "wav", "flac", "acc","m4a"
	};
	static unordered_set<QString> lrcFile{
		"txt","lrc"
	};

	static void PlayerInit() {
		bwdTime = fwdTime = 3.0;
		player = new QMediaPlayer;
		audioOutput = new QAudioOutput;
		player->setAudioOutput(audioOutput);
	}
	static void MusicInput(QUrl& url) {
		Player::player->setSource(url);
		Player::musicName = url.fileName();
	}
}