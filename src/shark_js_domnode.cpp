//
// Copyright 2010-2014 Jacob Dawid <jacob.dawid@cybercatalyst.net>
//
// This file is part of Shark.
//
// Shark is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Shark is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shark.  If not, see <http://www.gnu.org/licenses/>.
//

// Own includes
#include "shark_js_domnode.h"

namespace Shark {

namespace Js {

const QString DomNode::NodeType::Element = "element";
const QString DomNode::NodeType::Attribute = "attribute";
const QString DomNode::NodeType::Text = "text";
const QString DomNode::NodeType::CDATASection = "cdata-section";
const QString DomNode::NodeType::EntityReference = "entity-reference";
const QString DomNode::NodeType::Entity = "entity";
const QString DomNode::NodeType::ProcessingInstruction = "processing-instruction";
const QString DomNode::NodeType::Comment = "comment";
const QString DomNode::NodeType::Document = "document";
const QString DomNode::NodeType::DocumentType = "document-type";
const QString DomNode::NodeType::DocumentFragment = "document-fragment";
const QString DomNode::NodeType::Notation = "notation";
const QString DomNode::NodeType::Base = "base";
const QString DomNode::NodeType::CharacterData = "character-data";
const QString DomNode::NodeType::Unknown = "unknown";

DomNode::DomNode(Engine &engine, QDomDocument& domDocument, QDomNode domNode, QObject *parent)
    : QObject(parent),
      Logger("Shark::Js::DomNode"),
      _engine(engine),
      _domDocument(domDocument) {
    _domNode = domNode;
}

QJSValue DomNode::createElementBelow(QString tagName) {
    QDomElement element = _domDocument.createElement(tagName);
    QDomNode node =_domNode.insertAfter(element, _domNode.lastChild());
    return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
}

QJSValue DomNode::createTextBelow(QString data) {
    QDomText text = _domDocument.createTextNode(data);
    QDomNode node =_domNode.insertAfter(text, _domNode.lastChild());
    return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
}

QJSValue DomNode::createElementBefore(QString tagName) {
    if(!_domNode.parentNode().isNull()) {
        QDomElement element = _domDocument.createElement(tagName);
        QDomNode node =_domNode.parentNode().insertBefore(element, _domNode);
        return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::createTextBefore(QString data) {
    if(!_domNode.parentNode().isNull()) {
        QDomText text = _domDocument.createTextNode(data);
        QDomNode node =_domNode.parentNode().insertBefore(text, _domNode);
        return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::createElementAfter(QString tagName) {
    if(!_domNode.parentNode().isNull()) {
        QDomElement element = _domDocument.createElement(tagName);
        QDomNode node =_domNode.parentNode().insertAfter(element, _domNode);
        return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::createTextAfter(QString data) {
    if(!_domNode.parentNode().isNull()) {
        QDomText text = _domDocument.createTextNode(data);
        QDomNode node =_domNode.parentNode().insertAfter(text, _domNode);
        return _engine.toJSValue(new DomNode(_engine, _domDocument, node));
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::elements() {
    QList<QDomElement> searchResults;
    QDomElement domElement = _domNode.firstChildElement();
    while(domElement.isElement()) {
        searchResults.append(domElement);
        domElement = domElement.nextSiblingElement();
    }

    QJSValue searchResultsArray = _engine.createArray();
    int index = 0;
    foreach(QDomElement domElement, searchResults) {
        searchResultsArray.setProperty(index, _engine.toJSValue(new DomNode(_engine, _domDocument, domElement)));
        index++;
    }

    return searchResultsArray;
}

QJSValue DomNode::elementsByTagName(QString tagName) {
    QList<QDomElement> searchResults;
    QDomElement domElement = _domNode.firstChildElement(tagName);
    while(domElement.isElement()) {
        searchResults.append(domElement);
        domElement = domElement.nextSiblingElement(tagName);
    }

    QJSValue searchResultsArray = _engine.createArray();
    int index = 0;
    foreach(QDomElement domElement, searchResults) {
        searchResultsArray.setProperty(index, _engine.toJSValue(new DomNode(_engine, _domDocument, domElement)));
        index++;
    }

    return searchResultsArray;
}

QJSValue DomNode::elementsByClass(QString className) {
    QList<QDomElement> searchResults;
    QDomElement domElement = _domNode.firstChildElement();
    while(domElement.isElement()) {
        searchResults.append(domElement);
        domElement = domElement.nextSiblingElement();
    }

    QJSValue searchResultsArray = _engine.createArray();
    int index = 0;
    foreach(QDomElement domElement, searchResults) {
        DomNode *domNode = new DomNode(_engine, _domDocument, domElement);
        if(domNode->hasClass(className)) {
            searchResultsArray.setProperty(index, _engine.toJSValue(new DomNode(_engine, _domDocument, domElement)));
            index++;
        } else {
            delete domNode;
        }
    }

    return searchResultsArray;
}

QJSValue DomNode::addClass(QString className) {
    if(_domNode.isElement()) {
        if(className.split(QRegExp("\\s+"), QString::SkipEmptyParts).count() > 1) {
            // TODO: Find out how to throw an exception from C++ into JS
            // Invalid argument for class
        } else {
            QDomElement element = _domNode.toElement();

            if(element.hasAttribute("class")) {
                QString classString = element.attribute("class");
                QStringList classStringList = classString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                if(!classStringList.contains(className)) {
                    classStringList.append(className);
                    element.setAttribute("class", classStringList.join(" "));
                }
            } else {
                element.setAttribute("class", className);
            }

            return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode));
        }
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::removeClass(QString className) {
    if(_domNode.isElement()) {
        if(className.split(QRegExp("\\s+"), QString::SkipEmptyParts).count() > 1) {
            // TODO: Find out how to throw an exception from C++ into JS
            // Invalid argument for class
        } else {
            QDomElement element = _domNode.toElement();

            if(element.hasAttribute("class")) {
                QString classString = element.attribute("class");
                QStringList classStringList = classString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                if(classStringList.contains(className)) {
                    classStringList.removeAll(className);
                    element.setAttribute("class", classStringList.join(" "));
                }
            }

            return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode));
        }
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::toggleClass(QString className) {
    if(_domNode.isElement()) {
        if(className.split(QRegExp("\\s+"), QString::SkipEmptyParts).count() > 1) {
            // TODO: Find out how to throw an exception from C++ into JS
            // Invalid argument for class
        } else {
            if(hasClass(className)) {
                removeClass(className);
            } else {
                addClass(className);
            }
            return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode));
        }
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QJSValue DomNode::withClass(QString className) {
    return addClass(className);
}

bool DomNode::hasClass(QString className) {
    if(_domNode.isElement()) {
        if(className.split(QRegExp("\\s+"), QString::SkipEmptyParts).count() > 1) {
            // TODO: Find out how to throw an exception from C++ into JS
            // Invalid argument for class
        } else {
            QDomElement element = _domNode.toElement();

            if(element.hasAttribute("class")) {
                QString classString = element.attribute("class");
                QStringList classStringList = classString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                if(classStringList.contains(className)) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    } else {
        // TODO: Find out how to throw an exception from C++ into JS
    }

    return QJSValue::UndefinedValue;
}

QString DomNode::nodeName() {
    return _domNode.nodeName();
}

QString DomNode::nodeType() {
    switch(_domNode.nodeType()) {
    case QDomNode::ElementNode: return NodeType::Element;
    case QDomNode::AttributeNode: return NodeType::Attribute;
    case QDomNode::TextNode: return NodeType::Text;
    case QDomNode::CDATASectionNode: return NodeType::CDATASection;
    case QDomNode::EntityReferenceNode: return NodeType::EntityReference;
    case QDomNode::EntityNode: return NodeType::Entity;
    case QDomNode::ProcessingInstructionNode: return NodeType::ProcessingInstruction;
    case QDomNode::CommentNode: return NodeType::Comment;
    case QDomNode::DocumentNode: return NodeType::Document;
    case QDomNode::DocumentTypeNode: return NodeType::DocumentType;
    case QDomNode::DocumentFragmentNode: return NodeType::DocumentFragment;
    case QDomNode::NotationNode: return NodeType::Notation;
    case QDomNode::BaseNode: return NodeType::Base;
    case QDomNode::CharacterDataNode: return NodeType::CharacterData;
    }
    return NodeType::Unknown;
}

QJSValue DomNode::parentNode() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.parentNode()));
}

QJSValue DomNode::childNodes() {
    /* TODO: Implement. */
    return QJSValue();
}

QJSValue DomNode::firstChild() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.firstChild()));
}

QJSValue DomNode::lastChild() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.lastChild()));
}

QJSValue DomNode::previousSibling() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.previousSibling()));
}

QJSValue DomNode::nextSibling() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.nextSibling()));
}

QJSValue DomNode::attributes() {
    QDomNamedNodeMap namedNodeMap = _domNode.attributes();
    int count = namedNodeMap.count();
    QJSValue attributesArray = _engine.createArray();
    for(int index = 0; index < count; index++) {
         attributesArray.setProperty(index, namedNodeMap.item(index).nodeName());
    }
    return attributesArray;
}

QJSValue DomNode::ownerDocument() {
    return _engine.toJSValue(new DomNode(_engine, _domDocument, _domNode.ownerDocument()));
}

QString DomNode::namespaceURI() {
    return _domNode.namespaceURI();
}

QString DomNode::localName() {
    return _domNode.localName();
}

bool DomNode::hasAttributes() {
    return _domNode.hasAttributes();
}

bool DomNode::isAttr() {
    return _domNode.isAttr();
}

bool DomNode::isCDATASection() {
    return _domNode.isCDATASection();
}

bool DomNode::isDocumentFragment() {
    return _domNode.isDocumentFragment();
}

bool DomNode::isDocument() {
    return _domNode.isDocument();
}

bool DomNode::isDocumentType() {
    return _domNode.isDocumentType();
}

bool DomNode::isElement() {
    return _domNode.isElement();
}

bool DomNode::isEntityReference() {
    return _domNode.isEntityReference();
}

bool DomNode::isText() {
    return _domNode.isText();
}

bool DomNode::isEntity() {
    return _domNode.isEntity();
}

bool DomNode::isNotation() {
    return _domNode.isNotation();
}

bool DomNode::isProcessingInstruction() {
    return _domNode.isProcessingInstruction();
}

bool DomNode::isCharacterData() {
    return _domNode.isCharacterData();
}

bool DomNode::isComment() {
    return _domNode.isComment();
}

} // namespace Js

} // namespace Shark