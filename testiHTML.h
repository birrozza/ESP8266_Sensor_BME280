String poweredBy = 
                String ("<div style=\"text-align: right;") +    
                                     "background: linear-gradient(to left, #c0c0c0 0%,#73172f 100%); " +
                                     "color: #fff; " +
                                     "padding:10px; " +
                                     "vertical-align: middle;\">" +
                                          "<span style=\"color: #fff333;\" >Powered by A. Mirabelli</span>  " +                          
                        "</div>";


String sitoNonTrovato = // 404 sito non trovato
                String ("<html><body><h1 style=\"text-align: center; background-color: #C0BEA8;\">") + 
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">-= 404 =-</span>" +
                            "</strong>" +
                        "</h1>" +
                        "<h1 style=\"text-align: center; background-color: #c0c0c0;\">" +
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">Pagina non trovata !!!</span>" +
                            "</strong>" + 
                        "</h1>" + poweredBy + "</body></html>" ;

String unauthorized   =  // 401 non autorizzato
                String (/*<html><body>*/"<h1 style=\"text-align: center; background-color: #C0BEA8;\">") + 
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">-= 401 =-</span>" +
                            "</strong>" +
                        "</h1>" +
                        "<h1 style=\"text-align: center; background-color: #c0c0c0;\">" +
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">Unauthorized !!!</span>" +
                            "</strong>" + 
                        "</h1>" + poweredBy  /*+ "</body></html>"*/ ;
                       
String invalidRequest = // 400 invalid request
                String (/*<html><body>*/"<h1 style=\"text-align: center; background-color: #C0BEA8;\">") + 
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">-= 400 =-</span>" +
                            "</strong>" +
                        "</h1>" +
                        "<h1 style=\"text-align: center; background-color: #c0c0c0;\">" +
                            "<strong>" +
                                  "<span style=\"color: #ff0000;\">Invalid request !!!</span>" +
                            "</strong>" + 
                        "</h1>" + poweredBy /*+ "</body></html>"*/ ;
/*
String WebPageInit = String ("");

String WebPageClose = "";

String wifichart=String("");
*/
