[x] memory leeks
[x] link betwee response and Server
[x] link between CGI and request and response
    [x] check is CGI working correctly for just run cgi
    [x] for upload cgi and delete cgi its normal request that can be handeled by response
    [x] test CGI basic requests get
    [x] test CGI post request
    [x] CGI output should i parse its headers ? print(do it): print(don't do it) ==> do it
[x] hande GET request with body included

[ ] for every request there is on match location, i should just copy it one time into a attribute inside reqeust and use it when need, and for that i need:
        [ ] update getMatchLocaiton to return referenece to the Location, and after that copy by value that Location
        [ ] remove getMatchLocation or put in private
        [ ] update code to use getMatchLoc instead which is getter to the _match_loc
        [ ] Important: the logic now is working but imagine that you are using a pointer of variable in stack that can be gone at anytime !!!!!! SEGV !!!!! 
Tasks: 
   - [x] redirect @Alaoui
   - [x] Support cookies and session management (provide simple examples) @ali
        [x] created session management system
        [x] create UI page
        [x] link cookies part @ali + @alaoui (response) + @zobair(cgi)
        [x] test
   - [x] run CGI get/post pipline @zobair
   - [x] add ENV vars to CGI @zobair
   - [x] update is CGI to work to be true just with run CGI requests @zobair
   - [x] handel upload/delete cgi from folder /cgi-bin @alaoui
   - [x] parse CGI header output logic @zobair
   - [x] add test CGI UI page, (just AI part not that hard) @zobair
   - [ ] test if upload_store and root paths is exist, if not program should exit @Ali
   - [x] move default system to SETTING from RESPONSE @zobair 

   - [x] check memory leaks and tests @allllll
   - [ ] check Subject && code line by line @all (your part of code)
   - [ ] final tests 
   - [ ] create READMEs
   - [ ] upload project



   [ ] checking why the always large created file empty why read return 0
