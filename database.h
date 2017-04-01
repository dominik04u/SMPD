#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>

#include "object.h"

class Database
{
private:
    std::vector<Object> objects;
    std::map<std::string, int> classCounters;
	std::vector<std::string> classNamesVector;
    std::vector<unsigned int> featuresIDs;

    std::vector<Object> shuffledObjects;
    std::vector<Object> trainSet;
    std::vector<Object> testSet;

    unsigned int noClass;
    unsigned int noObjects;
    unsigned int noFeatures;

public:

    Database() : noClass(0), noObjects(0), noFeatures(0)
    {
    }

    bool addObject(const Object &object);
    void clear();
    bool load(const std::string &fileName);
    void save(const std::string &fileName);

    void train(int trainPartPercent);

    const std::vector<Object> &getObjects() const;
	const std::map<std::string, int>& getClassCounters() const { return classCounters; }
	const std::vector<std::string>& getClassNames() const { return classNamesVector; }

    const std::vector<Object> &getTrainSet() const;
    const std::vector<Object> &getTestSet() const;

    unsigned int getNoClass();
    unsigned int getNoObjects();
    unsigned int getNoFeatures();

};


#endif // DATABASE_H
