//
//
//

var arr;

var sort_by = 'value'; /*or key*/
var sort_dir_key = 'desc'; /*or desc*/
var sort_dir_value = 'asc'; /*or desc*/
var sort_icon_asc = '&#x2C4;';
var sort_icon_desc = '&#x2C5;';

$ ( function() {

    bind_btn_stats();
    $(".btn_stats").click();
    bind_txt_change();
    bind_btn_save();
    load_proj();

    rtl && $('.txt').css('direction','rtl');
    bind_ctrlS();
});

function bind_ctrlS(){

    var key;
    $(window).keydown(function(eevent) {

        if ( !( ((key=eevent.which) === 115 || key==83) && (eevent.ctrlKey || eevent.metaKey)) && !(eevent.which === 19)) return true;
        // alert("Ctrl/Cmd-S pressed");
        eevent.preventDefault();
        $('.btn_save').click();
        return false;
    });
}

var proj = qry('proj') || 'jimarata';
var rtl = qry('rtl') || 0;

function load_proj ( ) {

    var post_data = {
        proj: proj
    };

    $.post('server/load.php', post_data,
           function(data) {

               if ( data.err ) {
                   //err
                   console.error ( data.err );
               }
               //ok
               else {
                   console.log ( 'proj should be loaded OK', new Date );
                   $(".txt").val(data.text);
                   $(".txt").change();
               }
           }
          );
}

function bind_btn_save ( ) {

    $('.btn_save').click ( function(){

        var post_data = {
            proj: proj,
            text: $(".txt").val()
        };

        $.post('server/save.php', post_data,
               function(data) {

                   if ( data.err ) {

                       //err
                       console.error ( data.err );
                   }
                   //ok
                   else {
                       $(".txt").change();
                       console.log ( 'saved OK', new Date );
                   }
               }
              );
    });
}

function bind_txt_change(){

  $(".txt").change( function(){
    $(".btn_stats").click();
  } );
}

function bind_btn_stats(){

  $(".btn_stats").click( function(){
    stats_do();
  } );
}

//called from render do
function bind_sort(){

  $(".header").click( function(){

    var type=$(this).attr('type');
    sort_by = type;
    var sort_dir = window["sort_dir_"+type];
    sort_dir = (sort_dir==='asc') ? 'desc' : 'asc';
    window["sort_dir_"+type] = sort_dir;

    sort_do();
    render_do();
  } );
}

function sort_do ( ) {

  if(sort_by==='value'){

    arr.sort ( function(a, b){
      return sort_dir_value==='asc' ? a[1]-b[1] : b[1]-a[1];
    } );

  } else{
    var aa, bb, aaa, bbb;
    arr.sort ( function(a, b){

      aa = a[0], bb=b[0];

      if(sort_dir_key==='asc'){
        aaa = aa;
        bbb = bb;
      }
      else{
        bbb = aa;
        aaa = bb;
      }

      if (aaa < bbb) {
        return -1;
      }
      if (aaa > bbb) {
        return 1;
      }
      return 0;

    } );
  }
}

function render_do ( ) {

  $(".out").html("");
  var html = "<div type='key' class='fl key header hand' title='sort by character'>char " + ((sort_by==='key') ? window["sort_icon_"+sort_dir_key] : "") + "</div>";
  html += "<div type='value' class='fl value header hand'  title='sort by character count'>count "+ ((sort_by==='value') ? window["sort_icon_"+sort_dir_value] : "") +"</div>";
  html+="<div class='tbody'>";
  for ( var i=0; i<arr.length; ++i ) {
    html+="<div class='tr'>";
    html+="<div class='fl key'>"+arr[i][0]+"</div>";
    html+="<div class='fl value "+((arr[i][1]<5) ? 'alert' : '')+"'>"+arr[i][1]+"</div>";
    html+="<div class='cb'></div>";
    html+="</div>";
  }

  html+="</div>";
  $(".out").html(html);
  bind_sort();
}

function stats_do ( ) {

  var txt = $(".txt").val(), char_code;
  //console.log(txt);
  var obj = {};

  for ( var i=0; i<txt.length; ++i ) {
    if(/\s+/.test(txt[i])) continue;
    char_code = txt.charCodeAt(i).toString();
    !obj[char_code] && (obj[char_code]=0);
    obj[char_code]++;
  }

  arr = Object.keys(obj).reduce ( function (previous, key) {

    if ( key==8207 || key==8206 ) return previous;
    previous.push( [String.fromCharCode(key), obj[key], key] );
    return previous;
  }, [] );

  sort_do();
  render_do();
}
