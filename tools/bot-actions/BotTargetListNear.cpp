#include "BotTargetList.h"
#include "ui_BotTargetList.h"
#include "../../client/base/interface/DatapackClientLoader.h"
#include "../../client/fight/interface/ClientFightEngine.h"
#include "MapBrowse.h"

std::string BotTargetList::graphStepNearMap(const MultipleBotConnection::CatchChallengerClient * const client,const MapServerMini::BlockObject * const currentNearBlock,const unsigned int &depth)
{
    const std::unordered_set<const MapServerMini *> &validMaps=currentNearBlock->map->getValidMaps(depth);
    const std::unordered_set<const MapServerMini::BlockObject *> &accessibleBlock=currentNearBlock->map->getAccessibleBlock(validMaps,currentNearBlock);
    std::unordered_map<const MapServerMini::BlockObject *,MapServerMini::BlockObjectPathFinding> resolvedBlock;
    currentNearBlock->map->resolvBlockPath(currentNearBlock,resolvedBlock,accessibleBlock);

    // do the GraphViz content
    {
        std::string overall_graphvizText="";
        overall_graphvizText+="digraph G {\n";
        overall_graphvizText+="rankdir=LR\n";
        overall_graphvizText+="node [shape=record]\n";

        std::unordered_set<const MapServerMini::BlockObject *> destinationMaps;
        for(const auto& n : ActionsAction::actionsAction->map_list) {
            const MapServerMini * const mapServer=static_cast<MapServerMini *>(n.second);
            if(mapServer->step.size()>=2)
            {
                const MapServerMini::MapParsedForBot &step2=mapServer->step[1];
                if(step2.map!=NULL)
                {
                    unsigned int indexLayer=0;
                    while(indexLayer<step2.layers.size())
                    {
                        const MapServerMini::MapParsedForBot::Layer &layer=step2.layers.at(indexLayer);
                        const MapServerMini::BlockObject &block=*layer.blockObject;
                        for(const auto& n:block.links) {
                            const MapServerMini::BlockObject * const nextBlock=n.first;
                            if(validMaps.find(mapServer)!=validMaps.cend() && validMaps.find(nextBlock->map)!=validMaps.cend())
                                destinationMaps.insert(nextBlock);
                        }
                        indexLayer++;
                    }
                }
            }
        }

        std::string stringLinks;
        for(const auto& n : ActionsAction::actionsAction->map_list) {
            const MapServerMini * const mapServer=static_cast<MapServerMini *>(n.second);
            if(mapServer->step.size()>=2 && validMaps.find(mapServer)!=validMaps.cend())
            {
                const MapServerMini::MapParsedForBot &step2=mapServer->step[1];
                if(step2.map!=NULL)
                {
                    unsigned int contentDisplayed=0;
                    std::string subgraph;
                    subgraph+="subgraph cluster_"+std::to_string((uint64_t)mapServer)+" {\n";
                    subgraph+="label=\""+mapServer->map_file+"\";\n";
                    unsigned int indexLayer=0;
                    while(indexLayer<step2.layers.size())
                    {
                        const MapServerMini::MapParsedForBot::Layer &layer=step2.layers.at(indexLayer);
                        const MapServerMini::BlockObject &block=*layer.blockObject;

                        /*unsigned int index=0;
                        bool contentEmpty=true;
                        while(index<layer.contentList.size())
                        {
                            const MapServerMini::MapParsedForBot::Layer::Content &itemEntry=layer.contentList.at(index);
                            if(itemEntry.destinationDisplay==MapParsedForBot::Layer::DestinationDisplay::All)
                            {
                                contentEmpty=false;
                                break;
                            }
                            index++;
                        }
                        (void)contentEmpty;*/

                        bool haveValidDestination=false;
                        for(const auto& n:block.links) {
                            const MapServerMini::BlockObject * const nextBlock=n.first;
                            if(destinationMaps.find(nextBlock)!=destinationMaps.cend())
                            {
                                haveValidDestination=true;
                                break;
                            }
                        }

                        //layer.name!="Lost layer" || !contentEmpty ||
                        if(haveValidDestination || destinationMaps.find(&block)!=destinationMaps.cend())
                        {
                            if(accessibleBlock.find(&block)!=accessibleBlock.cend())
                            {
                                subgraph+="struct"+std::to_string((uint64_t)&block)+" [label=\"";
                                //subgraph+="<f0> "+mapServer->map_file+" | "+layer.name+" |";
                                subgraph+="<f0> "+layer.name;
                                /*unsigned int index=0;
                                while(index<layer.contentList.size())
                                {
                                    const MapServerMini::MapParsedForBot::Layer::Content &itemEntry=layer.contentList.at(index);
                                    if(itemEntry.destinationDisplay==MapParsedForBot::Layer::DestinationDisplay::All)
                                        subgraph+=itemEntry.text.toStdString()+"\\n";
                                    index++;
                                }*/
                                std::vector<std::string> stringList=contentToGUI(&block,client);
                                const std::string &content=stringimplode(stringList,"\\n");
                                if(!content.empty())
                                    subgraph+="|<f1> "+content;
                                {
                                    const MapServerMini::BlockObjectPathFinding &blockObjectPathFinding=resolvedBlock.at(&block);
                                    if(!blockObjectPathFinding.bestPath.empty())
                                    {
                                        subgraph+="|";
                                        unsigned int index=0;
                                        while(index<blockObjectPathFinding.bestPath.size())
                                        {
                                            const MapServerMini::BlockObject * const block=blockObjectPathFinding.bestPath.at(index);
                                            subgraph+="Block "+std::to_string(block->id+1)+"("+block->map->map_file+")\\n";
                                            index++;
                                        }
                                    }
                                }
                                subgraph+="\" style=filled fillcolor=\""+block.color.name(QColor::HexRgb).toStdString()+"\"]\n";
                                contentDisplayed++;
                            }
                        }

                        for(const auto& n:block.links) {
                            const MapServerMini::BlockObject * const nextBlock=n.first;
                            const MapServerMini::BlockObject::LinkInformation &linkInformation=n.second;
                            if(linkInformation.direction!=MapServerMini::BlockObject::LinkDirection::BothDirection || &block<=nextBlock)
                            {
                                if(validMaps.find(block.map)!=validMaps.cend() && validMaps.find(nextBlock->map)!=validMaps.cend())
                                    if(accessibleBlock.find(&block)!=accessibleBlock.cend() && accessibleBlock.find(nextBlock)!=accessibleBlock.cend())
                                    {
                                        contentDisplayed++;
                                        stringLinks+="struct"+std::to_string((uint64_t)&block)+" -> struct"+std::to_string((uint64_t)nextBlock);
                                        switch(linkInformation.direction)
                                        {
                                            case MapServerMini::BlockObject::LinkDirection::BothDirection:
                                                stringLinks+=" [dir=both];\n";
                                            break;
                                            default:
                                                stringLinks+=";\n";
                                            break;
                                        }
                                    }
                            }
                        }

                        indexLayer++;
                    }
                    subgraph+="}\n";
                    if(contentDisplayed>0)
                        overall_graphvizText+=subgraph;
                }
            }
        }

        overall_graphvizText+=stringLinks;
        overall_graphvizText+="}";
        return overall_graphvizText;
    }
    return "";
}

std::string BotTargetList::graphLocalMap()
{
    const QList<QListWidgetItem*> &selectedItems=ui->bots->selectedItems();
    if(selectedItems.size()!=1)
        return "";
    QListWidgetItem* selectedItem=selectedItems.at(0);
    const QString &pseudo=selectedItem->text();
    if(!pseudoToBot.contains(pseudo))
        return "";
    MultipleBotConnection::CatchChallengerClient * client=pseudoToBot.value(pseudo);
    if(!actionsAction->clientList.contains(client->api))
        return "";
    if(ui->comboBox_Layer->count()==0)
        return "";

    const ActionsBotInterface::Player &player=actionsAction->clientList.value(client->api);
    QString mapString="Unknown map ("+QString::number(mapId)+")";
    if(actionsAction->id_map_to_map.find(mapId)==actionsAction->id_map_to_map.cend())
        return "";
    const std::string &mapStdString=actionsAction->id_map_to_map.at(mapId);
    mapString=QString::fromStdString(mapStdString)+QString(" (%1,%2)").arg(player.x).arg(player.y);
    CatchChallenger::CommonMap *map=actionsAction->map_list.at(mapStdString);
    MapServerMini *mapServer=static_cast<MapServerMini *>(map);
    if((uint32_t)ui->comboBoxStep->currentIndex()>=mapServer->step.size())
        return "";
    MapServerMini::MapParsedForBot &step2=mapServer->step.at(ui->comboBoxStep->currentIndex());
    if(step2.map==NULL)
        return "";

    std::string graphvizText;
    // do the GraphViz content
    {
        graphvizText+="digraph G {\n";
        graphvizText+="rankdir=LR\n";
        graphvizText+="node [shape=record]\n";

        // the block
        std::unordered_map<const MapServerMini::BlockObject *,unsigned int> pointerToIndex;
        {
            unsigned int blockIndex=0;
            while(blockIndex<step2.layers.size())
            {
                const MapServerMini::MapParsedForBot::Layer &layer=step2.layers.at(blockIndex);
                const MapServerMini::BlockObject &block=*layer.blockObject;

                std::vector<std::string> stringList=contentToGUI(&block,client);
                const std::string &content=stringimplode(stringList,"\\n");

                /*unsigned int index=0;
                bool contentEmpty=true;
                while(index<layer.contentList.size())
                {
                    const MapServerMini::MapParsedForBot::Layer::Content &itemEntry=layer.contentList.at(index);
                    if(itemEntry.destinationDisplay==MapServerMini::MapParsedForBot::Layer::DestinationDisplay::All)
                    {
                        contentEmpty=false;
                        break;
                    }
                    index++;
                }*/

                if(layer.name!="Lost layer" || !content.empty())
                {
                    graphvizText+="struct"+std::to_string(blockIndex+1)+" [label=\"";
                    graphvizText+="<f0> "+layer.name;
                    //graphvizText+="<f1> "+layer.text;

                    if(!content.empty())
                    {
                        graphvizText+="|<f1> ";
                        graphvizText+=content;
                    }
                    graphvizText+="\" style=filled fillcolor=\""+block.color.name(QColor::HexRgb).toStdString()+"\"]\n";
                }
                pointerToIndex[&block]=blockIndex;
                blockIndex++;
            }
        }
        //the link
        {
            unsigned int blockIndex=0;
            while(blockIndex<step2.layers.size())
            {
                MapServerMini::BlockObject &block=*step2.layers[blockIndex].blockObject;

                for(const auto& n:block.links) {
                    const MapServerMini::BlockObject * const nextBlock=n.first;
                    const MapServerMini::BlockObject::LinkInformation &linkInformation=n.second;
//                    const std::vector<BlockObject::LinkSource> &sources=linkInformation.sources;
                    if(linkInformation.direction!=MapServerMini::BlockObject::LinkDirection::BothDirection || &block<=nextBlock || block.map!=nextBlock->map)
                    {
                        if(pointerToIndex.find(nextBlock)!=pointerToIndex.cend())
                            graphvizText+="struct"+std::to_string(blockIndex+1)+" -> struct"+std::to_string(pointerToIndex.at(nextBlock)+1);
                        else
                            graphvizText+="struct"+std::to_string(blockIndex+1)+" -> \""+nextBlock->map->map_file+", block "+std::to_string(nextBlock->id+1)+"\"";
                        switch(linkInformation.direction)
                        {
                            case MapServerMini::BlockObject::LinkDirection::BothDirection:
                                graphvizText+=" [dir=both];\n";
                            break;
                            default:
                                graphvizText+=";\n";
                            break;
                        }
                    }
                }

                blockIndex++;
            }
        }

        //graphvizText+="struct2 -> struct3 [dir=both];\n";

        graphvizText+="}";
    }
    return graphvizText;
}