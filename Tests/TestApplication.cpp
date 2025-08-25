#include <JuceHeader.h>
#include "TestRunner.h"

//==============================================================================
/**
    Standalone Test Application for AI Band Plugin
    
    This is a console application that runs all tests for the AI Band Plugin.
    It can be built and run independently of the plugin to verify functionality.
*/
class TestApplication : public juce::JUCEApplication
{
public:
    //==============================================================================
    TestApplication() = default;

    const juce::String getApplicationName() override
    {
        return "AI Band Plugin Tests";
    }

    const juce::String getApplicationVersion() override
    {
        return "1.0.0";
    }

    bool moreThanOneInstanceAllowed() override
    {
        return true;
    }

    //==============================================================================
    void initialise(const juce::String& commandLine) override
    {
        DBG("AI Band Plugin Test Application Starting...");
        DBG("Command line: " << commandLine);
        
        // Parse command line arguments
        juce::StringArray args;
        args.addTokens(commandLine, true);
        
        bool runSpecificSuite = false;
        juce::String suiteName;
        bool generateReport = false;
        
        for (int i = 0; i < args.size(); ++i)
        {
            if (args[i] == "--suite" && i + 1 < args.size())
            {
                runSpecificSuite = true;
                suiteName = args[i + 1];
                i++; // Skip next argument
            }
            else if (args[i] == "--report")
            {
                generateReport = true;
            }
            else if (args[i] == "--help" || args[i] == "-h")
            {
                printUsage();
                quit();
                return;
            }
        }
        
        // Run tests
        bool testsPassed = false;
        
        if (runSpecificSuite)
        {
            DBG("Running specific test suite: " << suiteName);
            testsPassed = TestRunner::runTestSuite(suiteName);
        }
        else
        {
            DBG("Running all test suites...");
            testsPassed = TestRunner::runAllTests();
        }
        
        // Generate report if requested
        if (generateReport)
        {
            auto report = TestRunner::runTestsWithReport();
            auto reportFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                             .getParentDirectory()
                             .getChildFile("test_report.txt");
            
            reportFile.replaceWithText(report);
            DBG("Test report saved to: " << reportFile.getFullPathName());
        }
        
        // Set exit code based on test results
        setApplicationReturnValue(testsPassed ? 0 : 1);
        
        DBG("Test application finished. Exit code: " << getApplicationReturnValue());
        quit();
    }

    void shutdown() override
    {
        DBG("Test application shutting down...");
    }

    //==============================================================================
    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        DBG("Another instance started with command line: " << commandLine);
    }

private:
    //==============================================================================
    void printUsage()
    {
        DBG("AI Band Plugin Test Application");
        DBG("Usage: TestApplication [options]");
        DBG("");
        DBG("Options:");
        DBG("  --suite <name>    Run specific test suite");
        DBG("  --report          Generate test report file");
        DBG("  --help, -h        Show this help message");
        DBG("");
        DBG("Available test suites:");
        auto suites = TestRunner::getAvailableTestSuites();
        for (auto& suite : suites)
        {
            DBG("  " << suite);
        }
        DBG("");
        DBG("Examples:");
        DBG("  TestApplication                    # Run all tests");
        DBG("  TestApplication --suite MidiManager # Run MidiManager tests only");
        DBG("  TestApplication --report           # Run all tests and generate report");
    }
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (TestApplication)
