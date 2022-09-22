void orders()
{    
    if (strncmp("NICK ", buffer, 5))
    {
        nickname();
    }
    memset(buffer,0,bufsize);
    send(client_socket[i], buffer,bufsize, 0);

}