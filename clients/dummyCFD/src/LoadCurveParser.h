#ifndef LOADCURVEPARSER_H_
#define LOADCURVEPARSER_H_

#include <fstream>
#include <sstream>
#include <string>
#include <assert.h>
#include <iostream>
#include <map>

using namespace std;

class LoadCurveParser {
public:
    LoadCurveParser(string loadCurveFileName) :
            timeToValueMap() {
        ifstream loadCurveFile(loadCurveFileName.c_str());
        string line;
        while (getline(loadCurveFile, line)) {
            double time;
            double value;

            stringstream ssLine(line);

            string timeString;
            ssLine >> timeString;
            timeString.erase(0, 5); // erase "TIME="
            stringstream ssTime(timeString);
            ssTime >> time;

            string valueString;
            ssLine >> valueString;
            valueString.erase(0, 4); // erase "VAL="
            stringstream ssValue(valueString);
            ssValue >> value;

            timeToValueMap.insert(pair<double, double>(time, value));
        }
        assert(timeToValueMap.size()>= 2);
    }
    virtual ~LoadCurveParser() {
    }
    double getValueAtTime(double time) {
        assert(time >= timeToValueMap.begin()->first);
        assert(time <= timeToValueMap.rbegin()->first);
        if (time == timeToValueMap.begin()->first)
            return timeToValueMap.begin()->second;
        if (time == timeToValueMap.rbegin()->first)
            return timeToValueMap.rbegin()->second;
        for (map<double, double>::iterator it = timeToValueMap.begin(); it != timeToValueMap.end();
                it++) {
            if (it->first > time) {
                it--;
                double time1 = it->first;
                double value1 = it->second;
                it++;
                double time2 = it->first;
                double value2 = it->second;
                return interpolate(time1, value1, time2, value2, time);
            }
        }
        return 12345.12345; // of course it is wrong
    }
    double interpolate(double x1, double y1, double x2, double y2, double x) {
        assert(x1!=x2);
        double y = (y2 - y1) / (x2 - x1) * (x - x1) + y1;
        return y;
    }
    void testInterpolate() {
        double y1 = interpolate(0.0, 0.0, 1.0, 1.0, 0.5);
        assert(y1==0.5);
        double y2 = interpolate(0.0, 0.0, 1.0, 1.0, 0.0);
        assert(y2==0);
        double y3 = interpolate(0.0, 0.0, 1.0, 1.0, 1.0);
        assert(y3==1.0);
        cout << endl << "testInterpolate: You are right!!!" << endl;
    }
    void printLoadCurve() {
        for (map<double, double>::iterator it = timeToValueMap.begin(); it != timeToValueMap.end();
                it++) {
            cout << "time=" << it->first << "\t" << "val=" << it->second << endl;
        }
    }
private:
    map<double, double> timeToValueMap;
};

#endif /* LOADCURVEPARSER_H_ */
