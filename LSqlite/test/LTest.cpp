//
// Created by 龙辉 on 16/4/3.
// Copyright (c) 2016 yuemo studio. All rights reserved.
//

#include "LTest.h"
#include "LSqliteCommand.h"

using namespace std;

LTest::LTest()
{

}

LTest::~LTest()
{

}

void LTest::test1()
{
    // initilize
    LSqlite::getInstance()->attach("/Users/longhui/Documents/longhui/github_dora/LSqlite/test.db");

    // command
    LSqliteCommand command("CREATE TABLE IF NOT EXISTS `test_records` (`id` INTEGER NOT NULL PRIMARY KEY);");
    bool ret = command.execute();

    command.reset("ALTER TABLE `test_records` ADD COLUMN `name` VARCHAR(255) DEFAULT 'unknown';");
    ret = command.execute();

    // non trasaction mult-commands
    clock_t tick1,tick2;
    tick1 = clock();
    command.clear();
    for (int i = 0; i < 100; ++i)
    {
        command.pushLine("INSERT OR REPLACE INTO `test_records` (`id`, `name`) VALUES(%d, '%s');");
        command.bindData(i, i+1, "longhui-A");
        //LLOG("s1 = %s", command.getSentence().c_str());
        command.execute();
    }
    tick2 = clock();
    LLOG("non transaction time used = %ld", tick2 - tick1);

    // trasaction used for fast
    tick1 = clock();
    command.clear();
    command.setTransactionEnabed(true);
    for (int i = 100; i < 200; ++i)
    {
        command.pushLine("INSERT OR REPLACE INTO `test_records` (`id`, `name`) VALUES(%d, '%s');");
        command.bindData(i, i+1, "longhui-B");
    }
    tick2 = clock();
    LLOG("transaction time used = %ld", tick2 - tick1);

    // command
    command.clear();
    command.setTransactionEnabed(false);
    command.reset("UPDATE `test_records` SET `name` = '%s' WHERE `id`= 8 OR `id`=72;");
    command.bindData(0, "dora");
    LLOG("s2 = %s", command.getSentence().c_str());
    command.execute();

    command.clear();
    command.reset("SELECT * FROM `test_records` WHERE `name` = 'dora'");
    command.execute([=](LSqliteResult * result){
        LLOG("data1-0: id = %s, name = %s", result->getValue("id").c_str(), result->getValue("name").c_str());
    });
}

void LTest::test2()
{
    // command
    LSqliteCommand command;
    command.attach("/Users/longhui/Documents/longhui/github_dora/LSqlite/test2.db");

    command.reset("CREATE TABLE IF NOT EXISTS `test_records_2` (`id` INTEGER NOT NULL PRIMARY KEY);");
    bool ret = command.execute();

    command.reset("ALTER TABLE `test_records_2` ADD COLUMN `version` VARCHAR(255) DEFAULT '1.0';");
    ret = command.execute();

    command.reset("INSERT OR REPLACE INTO `test_records_2` (`id`, `version`) VALUES(1, '0.2.18');");
    command.pushLine("INSERT OR REPLACE INTO `test_records_2` (`id`, `version`) VALUES(2, '1.2.2');");
    command.execute();

    command.reset("UPDATE `test_records_2` SET `version` = '%s' WHERE `id`= 20;");
    command.bindData(0, "3.12.0");
    LLOG("s3-0 = %s", command.getSentence().c_str());
    ret = command.execute();
    command.bindData(0, "4.0.1");
    LLOG("s3-1 = %s", command.getSentence().c_str());
    ret = command.execute();

    //
    command.reset("SELECT * FROM `test_records_2`;");
    LLOG("s4 = %s", command.getSentence().c_str());
    command.executeAsync([=](LSqliteResult * result){
        LLOG("column num = %d", result->getColumn());
        LLOG("data2: id = %s, version = %s", result->getValue("id").c_str(), result->getValue("version").c_str());
    });

}
