/*
 * Copyright 2014 Peng Wan
 *
 * This file is part of Qem.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef QEM_EPUB_H
#define QEM_EPUB_H

#include "book.h"
#include <zlib.h>


// import QuaZip
#ifndef QUAZIP_STATIC
# define QUAZIP_STATIC
#endif

class QuaZip;

QEM_BEGIN_NAMESPACE

namespace epub
{
    struct EpubMakeConfig
    {
        /// Directory of OPS(Open eBook Publication Structure)
        QString opsDir;
        /// Directory for storing book text.
        QString textDir;
        /// Directory for storing images.
        QString imageDir;
        /// Directory for storing styles files.
        QString styleDir;
        /// Encoding for all XML files.
        QByteArray xmlEncoding;
        /// Encoding for all HTML files.
        QByteArray htmlEncoding;
        /// ZIP compression method.
        int compressionMethod;
        /// ZIP compression level.
        int compressionLevel;
        /// main CSS file
        QString cssFile;
        /// CSS class name if main CSS file.
        QString coverStyle;

        QString introTitleStyle;
        QString introContentStyle;

        inline EpubMakeConfig(const QString &opsDir, const QString &textDir, const QString &imageDir,
                              const QString &styleDir, const QByteArray &xmlEncoding = QByteArray(),
                              const QByteArray &htmlEncoding = QByteArray()) :
            opsDir(opsDir), textDir(textDir), imageDir(imageDir), styleDir(styleDir), xmlEncoding(xmlEncoding),
            htmlEncoding(htmlEncoding), compressionMethod(Z_DEFLATED), compressionLevel(Z_DEFAULT_COMPRESSION),
            cssFile(":/mainCSS"), coverStyle("cover_div"), introTitleStyle("book_title_div"),
            introContentStyle("book_intro_div")
        {}
    };

    class QEM_SHARED_EXPORT EPUB
    {
    private:
        EPUB();
    public:
        /// Name of this book format.
        static const QString FORMAT_NAME;

        ///// MIME type for EPUB /////
        static const QString MIMETYPE_FILE;
        static const QByteArray MT_EPUB;

        // container.xml
        static const QString CONTAINER_FILE;
        static const QString CONTAINER_XML_NS;
        static const QString CONTAINER_VERSION;

        // CSS
        static QString CssSource;

        static QString CssFileName;
        static QString CssFileID;
        static const QString MT_CSS;

        // the Open Packaging Format (OPF)
        static QString OpfFileName;
        static const QString MT_OPF;
        static const QString OPF_XML_NS;
        static const QString OPF_VERSION_2;

        // Dublin Core Metadata Initiative (DCMI)
        static const QString DC_XML_NS;

        // dc:identifier element name
        static QString IdentifierName;

        // XML Schema instance
        //    static const QString XSI_XML_NS = "http://www.w3.org/2001/XMLSchema-instance";

        // the Navigation Center eXtended (NCX)
        static QString NcxFileName;
        static QString NcxFileId;

        static const QString MT_NCX;
        static const QString NCX_XML_DT;
        static const QString NCX_DT_ID;
        static const QString NCX_DT_URI;
        static const QString NCX_XML_NS;
        static const QString NCX_VERSION;

        //// Book cover image id
        static QString CoverFileID;

        // *******************
        // ** Book cover page
        // *******************
        static QString CoverPageTitle;
        static QString CoverPageFileName;
        static QString CoverPageFileId;
        /* CSS */
        static QString CoverPageImageStyle;


        // *******************
        // ** Book intro page
        // *******************
        static QString BookIntroText;
        static QString IntroPageTitle;
        static QString IntroPageFileName;
        static QString IntroPageFileId;
        /* CSS */
        static QString IntroPageTitleStyle;
        static QString IntroPageTextStyle;


        // *************************
        // ** Book information page
        // *************************
        static QString InfoPageTitle;
        static QString InfoPageFileName;
        static QString InfoPageFileId;
        /* CSS */
        static QString InfoPageTextStyle;

        // ******************
        // ** Book TOC page
        // ******************
        static QString TocPageTitle;
        static QString TocPageFileName;
        static QString TocPageFileId;
        /* CSS */
        static QString TocPageTitleStyle;
        static QString TocPageTextStyle;

        // last at the TOC HTML
        static QString BackToParentText;

        // ********************
        // ** Book index page
        // ********************
        static QString IndexPageTitle;
        static QString IndexPageFileName;
        static QString IndexPageFileId;

        /// Date format for metadata field.
        static const QString DATE_FORMAT;

        static const QString MT_XHTML;
        static const QString XHTML_NS;
        static const QString XHTML_DT;
        static const QString XHTML_DT_ID;
        static const QString XHTML_DT_URI;

        static const QString DUOKAN_FULL_SCREEN;

        /// Default EpubMakeConfig for ePub maker.
        static const EpubMakeConfig DefaultMakeConfig;

        /// Test given ZIP is ePub archive or not.
        static bool isEpub(QuaZip &zip);

        /// Qem Parser interface.
        static Book* parseEpub(QIODevice &device, const QVariantMap &args = QVariantMap(), QString *error = 0);

        static Book* parseEpub(QuaZip &zip, QString *error = 0);

        /// Qem Maker interface.
        static bool makeEpub(const Book &book, QIODevice &device, const QVariantMap &args = QVariantMap(),
                             QString *error = 0);

        static bool makeEpub(const Book &book, QuaZip &zip, const QString &version = QString("2"),
                             const EpubMakeConfig &config = DefaultMakeConfig, QString *error = 0);
    };
}   // epub

QEM_END_NAMESPACE

#endif // QEM_EPUB_H
