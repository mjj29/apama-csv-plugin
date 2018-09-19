# apama-csv-plugin
Apama EPL plugin and connectivity codec for parsing CSV data. Both the plugin and codec support user-defined delimiters and quoted fields which would otherwise contain the delimiter. Multiple rows are separated by newlines in the platform-specific newline format.

## Supported Apama version

This works with Apama 10.3.0.1 or 10.1.0.10 (or later fixes to either line)

## Building the plugin

In an Apama command prompt on Linux run:

    mkdir -p $APAMA_WORK/lib $APAMA_WORK/monitors
    g++ -std=c++11 -o $APAMA_WORK/lib/libCSVPlugin.so -I$APAMA_HOME/include -L$APAMA_HOME/lib -lapclient -I. -shared -fPIC CSVPlugin.cpp
	 cp eventdefinitions/CSVPlugin.mon $APAMA_WORK/monitors/CSVPlugin.mon

On Windows run:

    g++ -std=c++11 -o %APAMA_WORK%\lib\CSVPlugin.dll -I%APAMA_HOME%\include -L%APAMA_HOME%\lib -lapclient -I. -shared CSVPlugin.cpp
	 copy eventdefinitions\CSVPlugin.mon %APAMA_WORK%\monitors\CSVPlugin.mon

To generate the Apama documentation for the CSVPlugin module run this command on Linux:

    java -jar $APAMA_HOME/lib/ap-generate-apamadoc.jar doc eventdefinitions

Or on Windows:

    java -jar %APAMA_HOME%\lib\ap-generate-apamadoc.jar doc eventdefinitions

## Building using Docker

There is a provided Dockerfile which will build the plugin, run tests and produce an image which is your base image, plus the CSV plugin. Application images can then be built from this image. To build the image run:

    docker build -t apama_with_csv_plugin .

By default the public docker images from Docker Store for 10.3 will be used (once 10.3 has been released). To use an older version run:

    docker build -t apama_with_csv_plugin --build-arg APAMA_VERSION=10.1 .

To use custom images from your own repository then use:

    docker build -t apama_with_csv_plugin --build-arg APAMA_BUILDER=builderimage --build-arg APAMA_IMAGE=runtimeimage .

## Running tests

To run the tests for the plugin you will need to use an Apama command prompt.

You will need to compile a test transport:

    g++ -std=c++11 -o tests/libEchoTransport.so -I$APAMA_HOME/include -L$APAMA_HOME/lib -lapclient -I. -shared -fPIC EchoTransport.cpp

Then run the tests from within the tests directory:

    pysys run

## Use from EPL

From EPL you should inject the CSVPlugin.mon file, then create instances of the CSVPlugin class as decoders, configured with a separator. These instances can be used to encode and decode delimiter-separated values. These will be converted to a `sequence<sequence<string> >` type. For comma-separated use `CVSPlugin.createCSVDecoder()`:

    com.apamax.CSVPlugin csv := com.apamax.CSVPlugin.createCSVDecoder();
    
	 string text := input.text; // comma-separated
	 sequence<sequence<string> > data := csv.decode(text);
	 // ...
    text := csv.encode(data);

To specify your own delimiter use `CSVPlugin.createCustomDecoder()`:
    
    com.apamax.CSVPlugin tsv := com.apamax.CSVPlugin.createCustomDecoder("\t");
    
	 string text := input.text; // tab-separated
	 sequence<sequence<string> > data := tsv.decode(text);
	 // ...
    text := tsv.encode(data);

## Use from Connectivity plugins

As a codec in a connectivity chain you will need to first import the plugin into your configuration:

    connectivityPlugins:
	   csvCodec:
        libraryName: CSVPlugin
		  class: CSVCodec

You can now use the CSV codec in a chain definition:

    csvChain:
	    - apama.eventMap
       - mapperCodec:
		    # ... mapping configuration
		 - csvCodec
		 - stringCodec
		 - someTransport

The CSV codec expects a string payload on the transport side and a list[list[string]] payload on the host side. This will probably need to be mapped to/from an event field using the mapper codec before going to the host plugin.

The CSV codec takes the following options:

    csvChain
	   # ...
	   - csvCodec:
		    delimiter: '	' # custom delimiter, must be a single character. Default is ','
          filterOnContentType: true # if true, only apply to messages with the specified content type set. Default is false
			 contentType: text/tab-separated-values # the content type to filter and to set when formatted. Default is text/csv
	   # ...   

When filtering on content types you must ensure that the content type is correctly set on both messages towards host as well as messages towards transport. You can do this with the mapper codec:

    csvChain
	   # ...
	   - mapperCodec:
		    "*":
			    towardsTransport:
				    defaultValue:
					    - metadata.contentType: text/csv
		# ...
      - csvCodec:
		    filterOnContentType: true
	   # ...


