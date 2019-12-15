/*
 * main.cpp
 *
 *  Created on: 12/4/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include "Application.hpp"

int main(int argc, const char** argv)
{
  fixparser::cli::Application app;
  return app.run(argc, argv);
}

