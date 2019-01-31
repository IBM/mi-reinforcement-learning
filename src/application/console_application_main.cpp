/*!
 * Copyright (C) tkornuta, IBM Corporation 2015-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file console_application_main
 * \brief File contains "universal" main function for all applications using console logger.
 * \author tkornuta
 * \date Dec 30, 2015
 */

#include <application/Application.hpp>

#include <configuration/ParameterServer.hpp>

#include <logger/ConsoleOutput.hpp>
using namespace mic::logger;


/**
 * \brief Main function for all console-based applications.
 * \param[in] argc number of arguments
 * \param[in] argv list of arguments
 *
\dot
digraph finite_state_machine {
	size="15,15";
	node [shape = doublecircle]; LR_end;
	node [shape = circle];
	LR_0 [ label="Register\n application\n type" URL="\ref mic::applications::RegisterApplication"];
	LR_1 [ label="Create console\n logger" URL="\ref mic::configuration::logger:: ConsoleOutput"];
	LR_2 [ label="Parse\n program\n parameters" URL="\ref mic::configuration::ParameterServer::parseApplicationParameters"];
	LR_3 [ label="Initilize\n application\n state" URL="\ref mic::configuration::ApplicationState"];
	LR_4 [ label="Create\n application\n object" URL="\ref mic::configuration::ApplicationFactory::create"];
	LR_5 [ label="Initialize\n application\n parameters" URL="\ref mic::configuration::Application::initialize"];
	LR_6 [ label="Load\n properties\n from\n configuration\n file" URL="\ref mic::configuration::ParameterServer::loadPropertiesFromConfiguration"];
	LR_7 [ label="Initialize\n property\n dependent\n variables" URL="\ref mic::configuration::ParameterServer::initializePropertyDependentVariables"];
	LR_8 [ label="Run\n application" URL="\ref mic::configuration::Application::run"];
	LR_end [ label="end" ];

	LR_0 -> LR_1;
	LR_1 -> LR_2;
	LR_2 -> LR_3;
	LR_3 -> LR_4;
	LR_4 -> LR_5;
	LR_5 -> LR_6;
	LR_6 -> LR_7;
	LR_7 -> LR_8;
	LR_8 -> LR_end;
}
\enddot
*/

int main(int argc, char* argv[]) {
	// Register application/factory.
	mic::application::RegisterApplication();

	// Set console output to logger.
	LOGGER->addOutput(new ConsoleOutput());

	// Parse parameters.
	PARAM_SERVER->parseApplicationParameters(argc, argv);

	// Initilize application state ("touch it") ;)
	APP_STATE;

	mic::application::Application* app = APP_FACTORY->create(PARAM_SERVER->getAppName());

	// Initialize application.
	app->initialize(argc, argv);

	// Set parameters of all property-tree derived objects - USER independent part.
	PARAM_SERVER->loadPropertiesFromConfiguration();

	// Initialize property-dependent variables of all registered property-tree objects - USER dependent part.
	PARAM_SERVER->initializePropertyDependentVariables();

	// Run the application.
	app->run();
}
