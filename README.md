# apama-csv-plugin
Apama EPL plugin and connectivity codec for parsing CSV data. Both the plugin and codec support user-defined delimiters and quoted fields which would otherwise contain the delimiter. Multiple rows are separated by newlines in the platform-specific newline format.

## Buliding the plugin

In an Apama command prompt on Linux run:

   g++ -std=c++11 -o $APAMA\_WORK/lib/libCSVPlugin.so -I$APAMA\_HOME/include -L$APAMA\_HOME/lib -lapclient -I. -shared -fPIC CSVPlugin.cpp

On Windows run:

   g++ -std=c++11 -o %APAMA\_WORK%\lib\CSVPlugin.dll -I%APAMA\_HOME%\include -L%APAMA\_HOME%\lib -lapclient -I. -shared CSVPlugin.cpp

## Use from EPL

From EPL you should inject the CSVPlugin.mon file, then create instances of the CSVPlugin class as decoders, configured with a separator. These instances can be used to encode and decode delimiter-separated values. These will be converted to a `sequence<sequence<string> >` type. For comma-separated use `CVSPlugin.createCSVDecoder()`:

    CSVPlugin csv := CSVPlugin.createCSVDecoder();
    
	 string text := input.text; // comma-separated
	 sequence<sequence<string> > data := csv.decode(text);
	 // ...
    text := csv.encode(data);

To specify your own delimiter use `CSVPlugin.createCustomDecoder()`:
    
    CSVPlugin tsv := CSVPlugin.createCustomDecoder("\t");
    
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
		    delimiter: '	' # custom delimiter
	   # ...   


