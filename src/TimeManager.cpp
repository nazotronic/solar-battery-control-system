/*
 * Project: Solar Battery Control System
 *
 * Author: Vereshchynskyi Nazar
 * Email: verechnazar12@gmail.com
 * Version: 1.1.0
 * Date: 12.12.2024
 */

#include "data.h"

TimeManager::TimeManager() {
	makeDefault();
}

void TimeManager::begin() {
	clk.setUnix(0);
}


void TimeManager::tick() {
	if (ntp_flag) {
		if (millis() - ntp_sync_timer >= MIN_TO_MLS(NTP_SYNC_TIME) || !ntp_sync_timer) {
			NetworkManager* network = system->getNetworkManager();
			
			if (network->ntpSync(this)) {
				ntp_sync_timer = millis();
			}
		}
	}
}

void TimeManager::makeDefault() {
	system = NULL;

	ntp_flag = DEFAULT_NTP_FLAG;
	gmt = DEFAULT_GMT;
	ntp_sync_timer = 0;
}

void TimeManager::writeSettings(char* buffer) {
	setParameter(buffer, "STns", ntp_flag);
	setParameter(buffer, "STg", gmt);
}

void TimeManager::readSettings(char* buffer) {
	getParameter(buffer, "STns", &ntp_flag);
	getParameter(buffer, "STg", &gmt);
}

void TimeManager::addBlynkElements(BlynkManager* blynk) {
	blynk->addElement("P ntp sync", "STns", &ntp_flag, BLYNK_TYPE_BOOL);
	blynk->addElement("P gmt", "STg", &gmt, BLYNK_TYPE_INT8_T);
}


uint8_t TimeManager::status() {
	return clk.status();
}

uint8_t TimeManager::hour() {
	return clk.hour(gmt);
}

uint8_t TimeManager::minute() {
	return clk.minute(gmt);
}

uint8_t TimeManager::second() {
	return clk.second(gmt);
}

uint8_t TimeManager::weekday() {
	return clk.weekday(gmt);
}

uint8_t TimeManager::day() {
	return clk.day(gmt);
}

uint8_t TimeManager::month() {
	return clk.month(gmt);
}

uint16_t TimeManager::year() {
	return clk.year(gmt);
}


void TimeManager::setSystemManager(SystemManager* system) {
	this->system = system;
}


void TimeManager::setNtpFlag(bool ntp_flag) {
	this->ntp_flag = ntp_flag;
}

void TimeManager::setGmt(int8_t gmt) {
	this->gmt = gmt;
}

void TimeManager::setTime(TimeT* time) {
	clk.setTime(gmt, *time);
}

void TimeManager::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year) {
	clk.setTime(gmt, hour, minute, second, day, month, year);
}

void TimeManager::setUnix(uint32_t unix) {
	clk.setUnix(unix);
}


SystemManager* TimeManager::getSystemManager() {
	return system;
}

uint8_t TimeManager::getStatus() {
	return clk.status();
}

bool TimeManager::getNtpFlag() {
	return ntp_flag;
}

int8_t TimeManager::getGmt() {
	return gmt;
}

TimeT TimeManager::getTime() {
	return clk.getTime(gmt);
}

uint32_t TimeManager::getUnix() {
	return clk.getUnix();
}