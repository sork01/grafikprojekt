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
