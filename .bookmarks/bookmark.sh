#!/bin/bash
# *neode.command* setgo ¤bookmark

function entry {
    url="$1"
    title="$2"
    pic=$(echo "$url" | sed 's/[^a-z.]//g')".png"
    
    #echo '<div style="box-shadow: 0px 0px 15px #aaa; margin:12px; padding:8px; border:1px solid #bbb"><div style="float:left;clear:none;"><a href="'"$url"'"><img style="border:1px solid #777; width:320px" src=".bookmarks/'"$pic"'" /></a></div><div style="float:left;clear:none"><input class="url" type="text" style="width:318px" value="'"$url"'" /><br /><span class="title"><b>'"$title"'</b></span></div><div style="float:none;clear:both">&nbsp;</div></div>' >> bookmarks.html
    cat << EOT >> bookmarks.html
        <div class="bookmark" style="width:322px; height:216px; background:#fafafa; box-shadow: 0px 0px 15px #aaa; margin:24px; padding:8px; border:1px solid #aaf; float:left; clear:none">
            <div style="float:left;clear:none;">
                <div class="thetitle" style="width:318px; height:14pt; white-space:nowrap; overflow:hidden; margin-bottom:-12px; font:bold 12pt droid sans">
                    $title
                </div>
                <br />
                <a href="$url">
                    <img style="box-shadow:0px 0px 8px #aaa; border:1px solid #777; width:320px" src=".bookmarks/$pic" />
                </a>
            </div>
            <!--<div style="float:left; clear:none; margin-left:12px; width:100%">
                <input class="url" type="text" style="width:100%" value="$url" />
                <br />
                <span class="title" style="font:normal 18pt droid sans">
                    <b>$title</b>
                </span>
            </div>-->
        </div>
EOT
}

function html {
    rm -f bookmarks.html
    
    #echo '<!DOCTYPE html>' >> bookmarks.html
    #echo '<html><head><title>Bookmarks</title><script type="text/javascript" src=".bookmarks/jquery-3.2.0.min.js"></script><script type="text/javascript" src=".bookmarks/script.js"></script></head><body>' >> bookmarks.html
    
    cat << EOT >> bookmarks.html
<!DOCTYPE html>
<html style="overflow-y:scroll;">
    <head>
        <title>Bookmarks</title>
        <script type="text/javascript" src=".bookmarks/jquery-3.2.0.min.js"></script>
        <script type="text/javascript" src=".bookmarks/script.js"></script>
    </head>
    <body>
	<div style="width:400px; margin:0 auto;">
		<form id="filter">
			<input style="padding:6px; border:1px solid blue; font:normal 11pt droid sans; box-shadow:0px 0px 15px #aaa; width:400px;" name="filterstr" type="text" />
		</form>
	</div>
EOT
    
    OLDIFS=$IFS
    IFS=$'\n'
    
    for s in $(cat .bookmarks/bookmarks.txt | awk -F"|" '{ print $2 "|" $1 }' | sort | awk -F"|" '{ print $2"|"$1 }'); do
        url=$(echo "$s" | grep -Po '^[^|]+')
        title=$(echo "$s" | grep -Po '\|.+' | cut -c 2-)
        entry "$url" "$title"
    done
    
    IFS=$OLDIFS
    
    #echo '</body></html>' >> bookmarks.html
    cat << EOT >> bookmarks.html
    </body>
</html>
EOT

}

mkdir -p .bookmarks

if [ ! -f ".bookmarks/jquery-3.2.0.min.js" ]; then
    cd .bookmarks; wget https://code.jquery.com/jquery-3.2.0.min.js; cd ..
fi

if [ ! -f ".bookmarks/script.js" ]; then
    touch .bookmarks/script.js
    cat << 'EOT' > .bookmarks/script.js
$(document).ready(function()
{
    $("span.title>b:contains(#)").text(function() {
        return $(this).text().replace(/#[^\s]+/g, '');
    });
    
    $("form#filter input[name=filterstr]").bind("keyup", function() {
        var filterstr = $(this).val().toLowerCase();
        $("div.bookmark").each(function() {
            var title = $(this).find("div.thetitle").text().trim().toLowerCase();
            if (title.indexOf(filterstr) == -1)
            {
                $(this).hide();
            }
            else
            {
                $(this).show();
            }
        });
    });
});
EOT

fi

if [ ! "$1" == "" ]; then
    if [ "$2" == "" ]; then
        title="$1"
    else
        title="$2"
    fi
    
    echo "$1|$title" >> .bookmarks/bookmarks.txt
    
    cat .bookmarks/bookmarks.txt | sort | uniq > /tmp/bookmarks.txt
    cp /tmp/bookmarks.txt .bookmarks/bookmarks.txt
fi

OLDIFS=$IFS
IFS=$'\n'

for s in $(cat .bookmarks/bookmarks.txt); do
    s=$(echo "$s" | grep -Po '^[^|]+')
    picname=$(echo "$s" | sed 's/[^a-z.]//g')".png"
    
    if [ ! -f ".bookmarks/$picname" ]; then
        ¤webpage-screenshot 1024x600 "$s" ".bookmarks/$picname" >/dev/null 2>&1
    fi
done

IFS=$OLDIFS

html
