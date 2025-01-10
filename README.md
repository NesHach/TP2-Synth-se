# TP2-Synth-se
# readme 

TP2: Implementation of a TFTP Server and Client

Objective The main goal of this project was to design a program capable of communicating with a TFTP server while adhering to the specifications of the RFCs. The implemented functionalities include reading (RRQ) and writing (WRQ) files, as well as managing data packets, acknowledgments (ACK), and errors.

Main Features and Development Process
	•	1. Creation of TFTP Requests (RRQ and WRQ) Requests are constructed following the structure specified by the RFCs: Opcode: 01 for RRQ and 02 for WRQ. Filename followed by a null character ('\0'). Transfer mode (octet), also followed by a null character. Requests are sent to the server using UDP sockets via the sendto() function.
	•	2. Management of Data Packets and Acknowledgments (ACK) Files are divided into 512-byte blocks for transfer. Each block is sent with a unique block number (incremented). Acknowledgments (opcode 04) are verified for each received block. In case of an error, the program displays the received error message (opcode 05).
	•	3. Implementation on macOS Specific adjustments were required for macOS: Configuration of the integrated TFTP server: Modification of the system file /System/Library/LaunchDaemons/tftp.plist to add the --secure option. Creation of the shared directory /private/tftpboot with the necessary permissions. Use of launchctl commands to start and manage the TFTP server. Management of system rights (SIP): Temporary deactivation of SIP to modify protected files, followed by reactivation after the changes.
	•	4. Additional Features Execution time measurement: Measured using clock_gettime() to display the elapsed time for each transfer. Error and timeout handling: Implementation of a timeout to wait for server responses. Verification of server permissions and handling of access violations.

Skills Acquired
	•	1. System Programming Use of system calls: socket, sendto, recvfrom, fork, and execvp. File and permission management with standard C libraries.
	•	2. Network Communication Understanding and implementing UDP and TFTP protocols to ensure reliable data transmission despite the lack of persistent connection.
	•	3. Troubleshooting macOS-specific issues Handling system-specific restrictions such as SIP and differences in network behavior.
	•	4. Debugging and Error Management Diagnosing and resolving issues related to permissions, server configuration, and platform-specific differences.

Challenges Faced
	•	Configuring and launching the integrated TFTP server on macOS, including managing system permissions.
	•	Detecting and fixing network errors such as “Access violation” during transfer tests.
	•	Handling protected files requiring temporary SIP deactivation.
	•	Debugging protocol-specific behaviors, such as managing incorrect ACKs or packet loss.
