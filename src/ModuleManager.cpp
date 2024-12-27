/*
 * Project: Solar Battery Control System
 *
 * Author: Vereshchynskyi Nazar
 * Email: verechnazar12@gmail.com
 * Version: 1.2.0
 * Date: 27.12.2024
 */

#include "data.h"

ModuleManager::ModuleManager() {
	makeDefault();
}

void ModuleManager::begin() {
	oneWire.begin(DS18B20_PORT);
	ds18b20_sensor.setOneWire(&oneWire);
	
	ds18b20_sensor.begin();
	ds18b20_sensor.setResolution(12);
}


void ModuleManager::tick() {
	if (getReadDataTime()) {
		if (!read_data_timer || millis() - read_data_timer >= SEC_TO_MLS(getReadDataTime())) {
			read_data_timer = millis();
			updateModuleData();
		}
	}
}

void ModuleManager::makeDefault() {
	am2320_data.status = UNSPECIFIED_STATUS;

	memset(&am2320_data, 0, sizeof(am2320_data_t));
	memset(&ds18b20_data, 0, sizeof(ds18b20_data_t));

	for (uint8_t i = 0;i < getDS18B20Count();i++) {
		sprintf(ds18b20_data[i].name, "T%d", i + 1);
	
		ds18b20_data[i].read_attempts = DEFAULT_READ_ATTEMPTS;
		ds18b20_data[i].status = UNSPECIFIED_STATUS;
	}

	read_data_time = DEFAULT_READ_DATA_TIME;
	read_data_timer = 0;
}

void ModuleManager::writeSettings(char* buffer) {
	setParameter(buffer, "SMrdt", getReadDataTime());

	for (uint8_t i = 0;i < getDS18B20Count();i++) {
		setParameter(buffer, String("SMDSn") + i, (const char*) getDS18B20Name(i));
		setParameter(buffer, String("SMDSa") + i, getDS18B20Address(i), 8);
		setParameter(buffer, String("SMDSc") + i, getDS18B20Correction(i));
		setParameter(buffer, String("SMDSra") + i, getDS18B20ReadAttempts(i));
  	}
}

void ModuleManager::readSettings(char* buffer) {
	getParameter(buffer, "SMrdt", &read_data_time);
	
	for (byte i = 0;i < getDS18B20Count();i++) {
		getParameter(buffer, String("SMDSn") + i, ds18b20_data[i].name, 3);
		getParameter(buffer, String("SMDSa") + i, ds18b20_data[i].address, 8);
		getParameter(buffer, String("SMDSc") + i, &ds18b20_data[i].correction);
		getParameter(buffer, String("SMDSra") + i, &ds18b20_data[i].read_attempts);
  	}
}

void ModuleManager::addBlynkElements(BlynkManager* blynk) {
	blynk->addElement("T home", "HSt", &am2320_data.t, BLYNK_TYPE_FLOAT);
	blynk->addElement("H home", "HSh", &am2320_data.h, BLYNK_TYPE_FLOAT);

	for (uint8_t i = 0;i < getDS18B20Count();i++) {
		blynk->addElement(getDS18B20Name(i), String("HSdst") + i, &ds18b20_data[i].t, BLYNK_TYPE_FLOAT);
	}

	blynk->addElement("P data time", "SMrdt", &read_data_time, BLYNK_TYPE_UINT8_T);
}


void ModuleManager::setReadDataTime(uint8_t time) {
	read_data_time = constrain(time, 0, 100);
}


void ModuleManager::setDS18B20(uint8_t index, ds18b20_data_t* ds18b20) {
	if (ds18b20 == NULL) {
		return;
	}

	setDS18B20Name(index, ds18b20->name);
	setDS18B20Address(index, ds18b20->address);
	setDS18B20Correction(index, ds18b20->correction);
	setDS18B20ReadAttempts(index, ds18b20->read_attempts);
}

void ModuleManager::setDS18B20Name(uint8_t index, String name) {
	if (!isCorrectIndex(index)) {
		return;
	}
	
	name.toCharArray(ds18b20_data[index].name, 3);
}

void ModuleManager::setDS18B20Address(uint8_t index, uint8_t* address) {
	if (!isCorrectIndex(index)) {
		return;
	}

	memcpy(ds18b20_data[index].address, address, 8);
}

void ModuleManager::setDS18B20Correction(uint8_t index, float correction) {
	if (!isCorrectIndex(index)) {
		return;
	}

	ds18b20_data[index].correction = constrain(correction, -20.0, 20.0);
}

void ModuleManager::setDS18B20ReadAttempts(uint8_t index, uint8_t read_attempts) {
	if (!isCorrectIndex(index)) {
		return;
	}

	ds18b20_data[index].read_attempts = constrain(read_attempts, 0, 5);
}


DallasTemperature* ModuleManager::getDallasTemperature() {
	return &ds18b20_sensor;
}

uint8_t ModuleManager::getReadDataTime() {
	return read_data_time;
}


float ModuleManager::getAM2320T() {
	return am2320_data.t;
}

float ModuleManager::getAM2320H() {
	return am2320_data.h;
}

uint8_t ModuleManager::getAM2320Status() {
	return am2320_data.status;
}


uint8_t ModuleManager::getDS18B20Count() {
	return DS_SENSORS_COUNT;
}

ds18b20_data_t* ModuleManager::getDS18B20(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return NULL;
	}

	return ds18b20_data + index;
}

char* ModuleManager::getDS18B20Name(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return NULL;
	}

	return ds18b20_data[index].name;
}

uint8_t* ModuleManager::getDS18B20Address(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return NULL;
	}

	return ds18b20_data[index].address;
}

float ModuleManager::getDS18B20Correction(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return 0;
	}

	return ds18b20_data[index].correction;
}

uint8_t ModuleManager::getDS18B20ReadAttempts(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return 0;
	}

	return ds18b20_data[index].read_attempts;
}

float ModuleManager::getDS18B20T(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return 0;
	}

	return ds18b20_data[index].t + getDS18B20Correction(index);
}

uint8_t ModuleManager::getDS18B20Status(uint8_t index) {
	if (!isCorrectIndex(index)) {
		return UNSPECIFIED_STATUS;
	}

	return ds18b20_data[index].status;
}


void ModuleManager::updateModuleData() {
	am2320_data.status = am2320_sensor.read(&am2320_data.t, &am2320_data.h);

	for (uint8_t i = 0;i < getDS18B20Count();i++) {
		uint8_t attempts = ds18b20_data[i].read_attempts;

		if (!attempts) {
			ds18b20_data[i].status = UNSPECIFIED_STATUS;
		}

		while (attempts) {
			attempts--;

			ds18b20_sensor.requestTemperaturesByAddress(getDS18B20Address(i));
			ds18b20_data[i].t = ds18b20_sensor.getTempC(getDS18B20Address(i));

			if (getDS18B20T(i) < -100) {
				ds18b20_data[i].status = 1;
			}
			else if (getDS18B20T(i) == 85) {
				ds18b20_data[i].status = 2;
			}
			else {
				ds18b20_data[i].status = 0;
				break;
			}
		}
	}
}

bool ModuleManager::isCorrectIndex(uint8_t index) {
	if (index >= getDS18B20Count()) {
		return false;
	}

	return true;
}