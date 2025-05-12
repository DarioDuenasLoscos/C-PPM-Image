Project Statement:
Two independent executable programs will be developed: imtool-soa and imtool-aos, which will implement the SOA and AOS strategies, respectively.

• SOA – Structure of Arrays: The pixels of an image will be represented as three independent sequences. Each of the sequences will contain elements that can be in the range of 0 to 255 if the maximum intensity level is less than or equal to 255, or in the range of 0 to 65535 otherwise.

• AOS – Arrays of Structures: The pixels of an image will be represented as a single sequence of values. Each value in the sequence will consist of three fields that can be in the range of 0 to 255 or in the range of 0 to 65535 otherwise.

Application Parameters:
The application will take the following parameters:

• Input file path.

• Output file path.

• Operation to execute.

• Possible additional parameters depending on the operation.

The operation to be executed will be one of the following:

• info: Displays metadata information of the image to the standard output.

• maxlevel: Performs intensity scaling to a new maximum value. In this case, the new maximum value is supplied as an additional parameter.

• resize: Resizes the image. In this case, the new width and new height are supplied as additional parameters.

• cutfreq: Removes the least frequent colors. In this case, the number of additional values is supplied as additional parameters.

• compress: Compresses the image to the cppm format. In this case, there are no additional parameters.

IMPORTANT:
• If the number of arguments received by the application is less than three, an error message will be generated, and the program will terminate with the error code -1.

• If the number of arguments is equal to or greater than three, the third argument must be one of the following strings: info, maxlevel, resize, cutfreq, compress. Any other value as the third parameter will result in an error message being printed and generating the error code -1.

• If the option is info, the number of arguments must be exactly three. Otherwise, an error message will be generated, and the error code -1 will be returned.

• If the option is maxlevel, the number of arguments must be exactly four. The fourth argument must be an integer between the values 0 and 65535. Otherwise, an error message will be generated, and the error code -1 will be returned.

• If the option is resize, the number of arguments must be exactly five. The fourth argument must be a positive integer indicating the new width of the image. The fifth argument must be a positive integer indicating the new height of the image. Otherwise, an error message will be generated, and the error code -1 will be returned.

• If the option is cutfreq, the number of arguments must be exactly four. The fourth argument must be a positive integer. Otherwise, an error message will be generated, and the error code -1 will be returned.

• If the option is compress, the number of arguments must be exactly three. Otherwise, an error message will be generated, and the error code -1 will be returned.

Components:
The following components will be developed:

• common: Library with the source files common to both versions of the program (soa and aos).

• imgsoa: Library with all the image processing components using the soa strategy utilized from the main program.

• imgaos: Library with all the image processing components using the aos strategy utilized from the main program.

• utest-common: Unit tests for all components of the common library.

• utest-imgsoa: Unit tests for all components of the imgsoa library.

• utest-imgaos: Unit tests for all components of the imgaos library.

• ftest-soa: Functional tests for the imtool-soa application.

• ftest-aos: Functional tests for the imtool-aos application.

• imtool-soa: Executable program using the SOA strategy.

• imtool-aos: Executable program using the AOS strategy.
