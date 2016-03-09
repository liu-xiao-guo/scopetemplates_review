#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>

#include <iostream>
#include <QDebug>

namespace sc = unity::scopes;
using namespace sc;
using namespace std;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    // Support three different column layouts
    sc::ColumnLayout layout1col(1), layout2col(2), layout3col(3);

    // We define 3 different layouts, that will be used depending on the
    // device. The shell (view) will decide which layout fits best.
    // If, for instance, we are executing in a tablet probably the view will use
    // 2 or more columns.
    // Column layout definitions are optional.
    // However, we recommend that scopes define layouts for the best visual appearance.

    // Single column layout
    layout1col.add_column( { "image", "header", "summary", "tracks",
                             "videos", "gallery_header", "gallerys", "reviews", "exp"});

    // Two column layout
    layout2col.add_column( { "image" });
    layout2col.add_column( { "header", "summary", "tracks",
                              "videos", "gallery_header", "gallerys", "reviews","exp"});

    // Three cokumn layout
    layout3col.add_column( { "image" });
    layout3col.add_column( { "header", "summary", "tracks",
                             "videos", "gallery_header", "gallerys", "reviews", "exp"});
    layout3col.add_column( { });

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col, layout3col });

    // Define the header section
    sc::PreviewWidget header("header", "header");
    // It has title and a subtitle properties
    header.add_attribute_mapping("title", "title");
    header.add_attribute_mapping("subtitle", "subtitle");

    // Define the image section
    sc::PreviewWidget image("image", "image");
    // It has a single source property, mapped to the result's art property
    image.add_attribute_mapping("source", "art");

    // Define the summary section
    sc::PreviewWidget description("summary", "text");
    // It has a text property, mapped to the result's description property
    description.add_attribute_mapping("text", "description");

    Result result = PreviewQueryBase::result();
    PreviewWidget listen("tracks", "audio");
    {
        VariantBuilder builder;
        builder.add_tuple({
            {"title", Variant("This is the song title")},
            {"source", Variant(result["musicSource"].get_string().c_str())}
        });
        listen.add_attribute_value("tracks", builder.end());
    }

    PreviewWidget video("videos", "video");
    video.add_attribute_value("source", Variant(result["videoSource"].get_string().c_str()));
    video.add_attribute_value("screenshot", Variant(result["screenshot"].get_string().c_str()));

    sc::PreviewWidget header_gal("gallery_header", "header");
    header_gal.add_attribute_value("title", Variant("Gallery files are:"));

    PreviewWidget gallery("gallerys", "gallery");
//    gallery.add_attribute_value("sources", Variant(result["array"]));
    gallery.add_attribute_mapping("sources", "array");

    PreviewWidgetList widgets({ image, header, description });

    if ( result["musicSource"].get_string().length() != 0 ) {
        widgets.emplace_back(listen);
    }

    if( result["videoSource"].get_string().length() != 0 ) {
        widgets.emplace_back(video);
    }

    if( result["array"].get_array().size() != 0 ) {
        widgets.emplace_back(header_gal);
        widgets.emplace_back(gallery);
    }

    PreviewWidget review("reviews", "reviews");
//    w1.add_attribute_value("rating-icon-empty", Variant("file:///tmp/star-empty.svg"));
//    w1.add_attribute_value("rating-icon-full", Variant("file:///tmp/star-full.svg"));
    VariantBuilder builder;
    builder.add_tuple({
                          {"author", Variant("John Doe")},
                          {"review", Variant("very good")},
                          {"rating", Variant(3.5)}
                      });
    builder.add_tuple({
                          {"author", Variant("Mr. Smith")},
                          {"review", Variant("very poor")},
                          {"rating", Variant(5)}
                      });
    review.add_attribute_value("reviews", builder.end());
    widgets.emplace_back(review);

    PreviewWidget expandable("exp", "expandable");
    expandable.add_attribute_value("title", Variant("This is an expandable widget"));
    expandable.add_attribute_value("collapsed-widgets", Variant(1));
    PreviewWidget w1("w1", "text");
    w1.add_attribute_value("title", Variant("Subwidget 1"));
    w1.add_attribute_value("text", Variant("A text"));
    PreviewWidget w2("w2", "text");
    w2.add_attribute_value("title", Variant("Subwidget 2"));
    w2.add_attribute_value("text", Variant("A text"));
    expandable.add_widget(w1);
    expandable.add_widget(w2);

    widgets.emplace_back(expandable);

    reply->push( widgets );
}

