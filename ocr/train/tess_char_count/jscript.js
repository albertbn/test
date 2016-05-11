// -------------------------------------------------------------------
// ne (string is null or empty) - returns 1 if val is null or ''
// -------------------------------------------------------------------
function ne(val) {

    var ret = !(val!=null && ( val=='0' || val!=''));
    return ret;
}

// generic qry string
function qry(param, url /*expected in the form of &a=1&b=2...*/){

        if(url && !url.match(/\?/g) )  {
            url = '?d=1'+ url;
        }

        var loca = (ne(url)) ? document.location.href : url;

        if(loca.indexOf('?' + param + '=')>-1 || loca.indexOf('&' + param + '=')>-1){
                var qString = loca.split('?');
                var keyVal = qString[1].split('&');
                for(var i=0;i<keyVal.length;i++){
                        if(keyVal[i].indexOf(param + '=')==0){
                                var val = keyVal[i].split('=');
                                return val[1];
                        }
                }
                return null;
        }else{
                return null;
        }
}
