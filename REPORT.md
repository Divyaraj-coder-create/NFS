    Implementation of Network File System (NFS)
Handling of Naming Server
1> The Naming server code is written in main.c file in nm_server_client folder
2> The naming server(NM) continuously listens for incoming clients and servers in two separate "threads" named "listen_nm_thread" and "listen_client"
3> Whenever a storage server joins the NM , the NM recieves its credentials(like port_client and port_nm) and stores its entry in a global struct array "struct storage_node*"
4> Also , at this point , storage_node sends the list of its accessible files and folders (after reading from ss_x.txt) to the naming server.
5> The NM stores this list in a TRIE "struct trie_node*". each node of trie stores a character of a path and a value end_of_file , which is set to 1 when path ends.
6> The NM upon recieveing "CLOSE" request from a SS, closes that SS and deletes its entries from trie and cache.a
7> At client end.
8> Whenever a client approaches the NM, the nm accepts it and executes whatever operation it demands.
9> The NM code is also responsible for maintaining a Trie and Cache.a
10> Also , the NM searches in Trie and the corresponding port details and IP details of SS is sent to client.c in case the operation is READ/WRITE.



Handling of Storage_server:-
1> The storage server upon connection with NM , sends its accessible files and folders list to NM.
2> We created threads for continuous listening of client as well as NM server requests. Once SS recieves request from Nm server, it is because of create,delete,details or Copy operations.
3> If request is from  